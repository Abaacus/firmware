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

#define TRACTION_CONTROL_TASK_ID 3
#define TRACTION_CONTROL_TASK_PERIOD_MS 200
#define RPM_TO_RADS(rpm) ((rpm)*2*PI/60.0f)

// Macros for converting RPM to KPH
#define GEAR_RATIO 15.0/52.0
#define M_TO_KM 1.0/1000.0f
#define WHEEL_DIAMETER_M 0.52
#define WHEEL_CIRCUMFERENCE WHEEL_DIAMETER_M*PI
#define HOUR_TO_MIN 60
#define RPM_TO_KPH(rpm) ((rpm)*HOUR_TO_MIN*WHEEL_CIRCUMFERENCE*M_TO_KM*GEAR_RATIO)

// For every 1rad/s, decrease torque by kP
#define kP_DEFAULT (0.1f)

// With our tire radius, rads/s ~ km/h
#define ERROR_FLOOR_RADS_DEFAULT (20.0f)
#define ADJUSTMENT_TORQUE_FLOOR_DEFAULT (2.0f)

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
	return RPM_TO_RADS(ScaledMotorSpeedRight);
}

static float get_RL_speed()
{
	//Value comes from MC
	return RPM_TO_RADS(ScaledMotorSpeedLeft);
}

float kP = kP_DEFAULT;
float error_floor = ERROR_FLOOR_RADS_DEFAULT;
float adjustment_torque_floor = ADJUSTMENT_TORQUE_FLOOR_DEFAULT;

void tractionControlTask(void *pvParameters)
{
	if (registerTaskToWatch(TRACTION_CONTROL_TASK_ID, 2*pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS), false, NULL) != HAL_OK)
	{
		ERROR_PRINT("ERROR: Failed to init traction control task, suspending traction control task\n");
		while(1);
	}

	float torque_max = MAX_TORQUE_DEMAND_DEFAULT;
	float torque_adjustment = adjustment_torque_floor;
	float FR_speed = 0.0f; //front right wheel speed
	float FL_speed = 0.0f; //front left wheel speed
	float RR_speed = 0.0f; //rear right wheel speed
	float RL_speed = 0.0f; //rear left wheel speed
	float error_left = 0.0f; //error between left rear and front
	float error_right = 0.0f; //error between right rear and front
	TickType_t xLastWakeTime = xTaskGetTickCount();

	//initialized variables so that speed is 0 on startup 
	VCU_wheelSpeed_RR = MC_ENCODER_OFFSET;
	VCU_wheelSpeed_RL = MC_ENCODER_OFFSET; 

	while(1)
	{
		torque_max = MAX_TORQUE_DEMAND_DEFAULT;

		FR_speed = get_FR_speed(); 
		FL_speed = get_FL_speed(); 
		RR_speed = get_RR_speed(); 
		RL_speed = get_RL_speed(); 

		VCU_wheelSpeed_RR = RR_speed;
		sendCAN_TC_wheelSpeed_right();

		VCU_wheelSpeed_RL = RL_speed;
		sendCAN_TC_wheelSpeed_left();

		SpeedMotorRightKPH = RPM_TO_KPH(((int64_t)SpeedMotorRight) - MC_ENCODER_OFFSET);
		SpeedMotorLeftKPH = RPM_TO_KPH(((int64_t)SpeedMotorLeft) - MC_ENCODER_OFFSET);
		sendCAN_SpeedMotorKPH();

		if(tc_on)
		{
			torque_adjustment = 0.0f;

			error_left = RL_speed - FL_speed;
			error_right = RR_speed - FR_speed;

			//calculate error. This is a P-controller
			if(error_left > error_floor || error_right > error_floor)
			{
				if (error_left > error_right)
				{
					torque_adjustment = (error_left - error_floor) * kP;
				}
				else
				{
					torque_adjustment = (error_right - error_floor) * kP;
				}
			}

			Torque_Adjustment_Right = (uint32_t) (error_right * kP);
			Torque_Adjustment_Left = (uint32_t) (error_left * kP);
			sendCAN_TC_Torque_Adjustment_Left();
			sendCAN_TC_Torque_Adjustment_Right();

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
			Torque_Max = (uint32_t)torque_max;
			sendCAN_TC_Torque_Max();
		}

		setTorqueLimit(torque_max);

		// Always poll at almost exactly PERIOD
        watchdogTaskCheckIn(TRACTION_CONTROL_TASK_ID);
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS));
	}

}
