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
#define TC_kP_DEFAULT (8.0f)

// With our tire radius, rads/s ~ km/h
#define ERROR_FLOOR_RADS_DEFAULT (2.0f)
#define ADJUSTMENT_TORQUE_FLOOR_DEFAULT (0.0f)

typedef struct {
	float FL;
	float FR;
	float RL;
	float RR;
} WheelData_S;

typedef struct {
	float torque_max;
} TCData_S;

static bool tc_on = false;

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

	Torque_Max = tc_data->torque_max;
	sendCAN_TCTorqueMax();
}


float tc_kP = TC_kP_DEFAULT;
float error_floor = ERROR_FLOOR_RADS_DEFAULT;
float adjustment_torque_floor = ADJUSTMENT_TORQUE_FLOOR_DEFAULT;

void tractionControlTask(void *pvParameters)
{
	if (registerTaskToWatch(TRACTION_CONTROL_TASK_ID, 2*pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS), false, NULL) != HAL_OK)
	{
		ERROR_PRINT("ERROR: Failed to init traction control task, suspending traction control task\n");
		while(1);
	}

	WheelData_S wheel_data = {0};
	TCData_S tc_data = {0};

	float torque_max = MAX_TORQUE_DEMAND_DEFAULT;
	float torque_adjustment = adjustment_torque_floor;
	float error_left = 0.0f; //error between left rear and front
	float error_right = 0.0f; //error between right rear and front
	TickType_t xLastWakeTime = xTaskGetTickCount();

	//initialized variables so that speed is 0 on startup 
	SpeedMotorRight = MC_ENCODER_OFFSET;
	SpeedMotorLeft = MC_ENCODER_OFFSET; 

	while(1)
	{
		torque_max = MAX_TORQUE_DEMAND_DEFAULT;
		// rads/s
		wheel_data.FL = get_FL_speed(); 
		wheel_data.FR = get_FR_speed(); 
		wheel_data.RL = get_RL_speed(); 
		wheel_data.RR = get_RR_speed(); 


		if(tc_on)
		{
			torque_adjustment = 0.0f;

			error_left = wheel_data.RL - wheel_data.FL;
			error_right = wheel_data.RR - wheel_data.FR;

			//calculate error. This is a P-controller
			if(error_left > error_floor || error_right > error_floor)
			{
				if (error_left > error_right)
				{
					torque_adjustment = (error_left - error_floor) * tc_kP;
				}
				else
				{
					torque_adjustment = (error_right - error_floor) * tc_kP;
				}
			}

			//clamp values
			torque_max = MAX_TORQUE_DEMAND_DEFAULT - torque_adjustment;
			if(torque_max < adjustment_torque_floor)
			{
				torque_max = adjustment_torque_floor;
			}
			else if(torque_max > MAX_TORQUE_DEMAND_DEFAULT)
			{
				// Whoa error in TC (front wheel is spinning faster than rear)
				torque_max = MAX_TORQUE_DEMAND_DEFAULT;
			}
		}

		setTorqueLimit(torque_max);

		tc_data.torque_max = torque_max;
		publish_can_data(&wheel_data, &tc_data);
		// Always poll at almost exactly PERIOD
        watchdogTaskCheckIn(TRACTION_CONTROL_TASK_ID);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS));
	}

}
