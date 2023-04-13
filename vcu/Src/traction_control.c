#include "traction_control.h"
#include "stdint.h"
#include "stdbool.h"
#include "watchdog.h"
#include "debug.h"
#include "motorController.h"
#include "vcu_F7_can.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "stm32f7xx_hal_tim.h"

#define PI 3.14159

/*
The motor controllers will return a 16 bit unsigned integer that needs to be converted to an integer value with the middle being at 32768. Negative numbers mean the wheels are spinning backwards, Positive values indicate forward spin
This exists and isn't done in the DBC bc the CAN driver has issues with the order of casting gives us large numbers around the middle point when the speed is around 0 
We want to do (((int32_t)rpm) - 32768)  where the driver will do  (int32_t)((uint32_t)rpm-32768)
*/
#define MC_ENCODER_OFFSET 32768

#define TRACTION_CONTROL_TASK_ID 3
#define TRACTION_CONTROL_TASK_PERIOD_MS 20
#define RPM_TO_RADS(rpm) ((rpm)*2*PI/60.0f)

// Macros for converting RPM to KPH
#define GEAR_RATIO ((float)(15.0/52.0))
#define M_TO_KM 1.0/1000.0f
#define WHEEL_DIAMETER_M 0.525
#define WHEEL_CIRCUMFERENCE WHEEL_DIAMETER_M*PI
#define SECS_PER_HOUR (3600.0f)
#define RADS_TO_KPH(rads) (rads * (WHEEL_DIAMETER_M/2.0) * SECS_PER_HOUR * M_TO_KM)
// For every 1rad/s, decrease torque by kP
#define TC_kP_DEFAULT (1.0f)

// With our tire radius, rads/s ~ km/h
#define ERROR_FLOOR_RADS_DEFAULT (2.5f)
#define ADJUSTMENT_TORQUE_FLOOR_DEFAULT (4.0f)

typedef struct {
	float FL;
	float FR;
	float RL;
	float RR;
} WheelData_S;

typedef struct {
	float torque_max;
	float left_error;
	float right_error;
	float torque_adjustment;
	float cum_error;
} TCData_S;


static bool tc_on = false;

float tc_kP = TC_kP_DEFAULT;
float tc_kI = 0.0f;
float error_floor = ERROR_FLOOR_RADS_DEFAULT;
float adjustment_torque_floor = ADJUSTMENT_TORQUE_FLOOR_DEFAULT;

void disable_TC(void)
{
	tc_on = false;
}

void toggle_TC(void)
{
	tc_on = !tc_on;
}

static float get_FR_speed()
{
	//Value comes from WSB
	return FR_Speed;
}

static float get_FL_speed()
{
	//Value comes from WSB
	return FL_Speed;
}

static float get_RR_speed()
{
	//Value comes from MC
	int64_t val = SpeedMotorRight;
	return RPM_TO_RADS(val - MC_ENCODER_OFFSET)*GEAR_RATIO;
}

static float get_RL_speed()
{
	//Value comes from MC
	int64_t val = SpeedMotorLeft;
	return RPM_TO_RADS(val - MC_ENCODER_OFFSET)*GEAR_RATIO;
}

static void publish_can_data(WheelData_S* wheel_data, TCData_S* tc_data)
{	
	if(NULL == wheel_data || NULL == tc_data)
	{
		ERROR_PRINT("Null pointer passed to publish_can_data\n");
		Error_Handler();
	}
	VCU_wheelSpeed_RL = wheel_data->RL;
	VCU_wheelSpeed_RR = wheel_data->RR;
	sendCAN_RearWheelSpeedRADS();

	FLSpeedKPH = RADS_TO_KPH(wheel_data->FL);
	FRSpeedKPH = RADS_TO_KPH(wheel_data->FR);
	RLSpeedKPH = RADS_TO_KPH(wheel_data->RL);
	RRSpeedKPH = RADS_TO_KPH(wheel_data->RR);
	sendCAN_WheelSpeedKPH();

	TCTorqueMax = tc_data->torque_max;
	TCTorqueAdjustment = tc_data->torque_adjustment;
	TCLeftError = tc_data->left_error;
	TCRightError = tc_data->right_error;
	sendCAN_TractionControlData();
}

static float tc_compute_limit(WheelData_S* wheel_data, TCData_S* tc_data)
{
	if(NULL == wheel_data || NULL == tc_data)
	{
		ERROR_PRINT("Null pointer passed to tc_compute_limit\n");
		Error_Handler();
	}

	tc_data->torque_max = MAX_TORQUE_DEMAND_DEFAULT;
	tc_data->torque_adjustment = 0.0f;

	tc_data->left_error = wheel_data->RL - wheel_data->FL;
	tc_data->right_error = wheel_data->RR - wheel_data->FR;

	//calculate error. This is a P-controller
	float error = 0.0f;
	if(tc_data->left_error > error_floor || tc_data->right_error > error_floor)
	{
		if (tc_data->left_error > tc_data->right_error)
		{
			error = tc_data->left_error - error_floor;
		}
		else
		{
			error = tc_data->right_error - error_floor; 
		}
		tc_data->cum_error += error;
	}
	
	tc_data->torque_adjustment = tc_kP * error + tc_kI * tc_data->cum_error;
	//clamp values
	tc_data->torque_max = MAX_TORQUE_DEMAND_DEFAULT - tc_data->torque_adjustment;
	if(tc_data->torque_max < adjustment_torque_floor)
	{
		tc_data->torque_max = adjustment_torque_floor;
	}
	else if(tc_data->torque_max > MAX_TORQUE_DEMAND_DEFAULT)
	{
		// Whoa error in TC (front wheel is spinning faster than rear)
		tc_data->torque_max = MAX_TORQUE_DEMAND_DEFAULT;
	}
	return tc_data->torque_max;
}



void tractionControlTask(void *pvParameters)
{
	if (registerTaskToWatch(TRACTION_CONTROL_TASK_ID, 2*pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS), false, NULL) != HAL_OK)
	{
		ERROR_PRINT("ERROR: Failed to init traction control task, suspending traction control task\n");
		while(1);
	}

	WheelData_S wheel_data = {0};
	TCData_S tc_data = {0};

	tc_data.torque_max = MAX_TORQUE_DEMAND_DEFAULT;
	tc_data.torque_adjustment = adjustment_torque_floor;
	tc_data.left_error = 0.0f; 
	tc_data.right_error = 0.0f; 
	TickType_t xLastWakeTime = xTaskGetTickCount();

	//initialized variables so that speed is 0 on startup 
	SpeedMotorRight = MC_ENCODER_OFFSET;
	SpeedMotorLeft = MC_ENCODER_OFFSET; 

	while(1)
	{
		// rads/s
		wheel_data.FL = get_FL_speed(); 
		wheel_data.FR = get_FR_speed(); 
		wheel_data.RL = get_RL_speed(); 
		wheel_data.RR = get_RR_speed(); 
	

		float tc_torque = tc_compute_limit(&wheel_data, &tc_data);
		float output_torque = tc_on ? tc_torque : MAX_TORQUE_DEMAND_DEFAULT;

		setTorqueLimit(output_torque);

		publish_can_data(&wheel_data, &tc_data);
		// Always poll at almost exactly PERIOD
        watchdogTaskCheckIn(TRACTION_CONTROL_TASK_ID);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS));
	}

}
