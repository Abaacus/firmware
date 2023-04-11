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
#define TC_kP_DEFAULT (0.1f)

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

void set_TC_enabled(bool tc_bool)
{
	tc_on = tc_bool;
	DEBUG_PRINT("Setting tc_on to: %d, the tc_on is now: %d\r\n", tc_bool, tc_on);
}

bool get_TC(void)
{
	DEBUG_PRINT("TC_ON: %d (default false)\r\n", tc_on);
	return tc_on;
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
	return RPM_TO_RADS(val - MC_ENCODER_OFFSET);
}

static float get_RL_speed()
{
	//Value comes from MC
	int64_t val = SpeedMotorLeft;
	return RPM_TO_RADS(val - MC_ENCODER_OFFSET);
}

static float tc_kP = TC_kP_DEFAULT;
static float error_floor = ERROR_FLOOR_RADS_DEFAULT;
static float adjustment_torque_floor = ADJUSTMENT_TORQUE_FLOOR_DEFAULT;

void tractionControlTask(void *pvParameters)
{
	if (registerTaskToWatch(TRACTION_CONTROL_TASK_ID, 2*pdMS_TO_TICKS(TRACTION_CONTROL_TASK_PERIOD_MS), false, NULL) != HAL_OK)
	{
		ERROR_PRINT("ERROR: Failed to init traction control task, suspending traction control task\n");
		while(1);
	}
    
	float max_torque_demand = get_max_torque_demand();
	float torque_max = max_torque_demand;
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
		max_torque_demand = get_max_torque_demand();
		torque_max = max_torque_demand;

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
					torque_adjustment = (error_left - error_floor) * tc_kP;
				}
				else
				{
					torque_adjustment = (error_right - error_floor) * tc_kP;
				}
			}

			Torque_Adjustment_Right = (uint32_t) (error_right * tc_kP);
			Torque_Adjustment_Left = (uint32_t) (error_left * tc_kP);
			sendCAN_TC_Torque_Adjustment_Left();
			sendCAN_TC_Torque_Adjustment_Right();

			//clamp values
			torque_max = max_torque_demand - torque_adjustment;
			if(torque_max < adjustment_torque_floor)
			{
				torque_max = adjustment_torque_floor;
			}
			else if(torque_max > max_torque_demand)
			{
				// Whoa error in TC (front wheel is spinning faster than rear)
				torque_max = max_torque_demand;
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

#define TC_KP_BASE (0.001f)
#define TC_ERROR_FLOOR_BASE (0.001f)
#define ADJUSTMENT_TORQUE_FLOOR_BASE (0.001f)

HAL_StatusTypeDef set_tc_kP(float tc_kP_value)
{
	tc_kP_value *= TC_KP_BASE;
	if(tc_kP_value < 0 || tc_kP_value > 1)
	{
		ERROR_PRINT("Failed to set tc_kP value\nThe range is [0, 1]]\r\n");
		return HAL_ERROR;
	}
    tc_kP = tc_kP_value;
	DEBUG_PRINT("Setting tc_kP to: %f, the tc_kP is now: %f\r\n", tc_kP_value, tc_kP);
	return HAL_OK;
}

HAL_StatusTypeDef set_tc_error_floor(float error_floor_value)
{
	error_floor_value *= TC_ERROR_FLOOR_BASE;
	if(error_floor_value < 0)
	{
		ERROR_PRINT("Failed to set error_floor value\nThe range is [0,infinity)\r\n");
		return HAL_ERROR;
	}
    error_floor = error_floor_value;
	DEBUG_PRINT("Setting error_floor to: %f, the error_floorr is now: %f\r\n", error_floor_value, error_floor);
	return HAL_OK;
}

HAL_StatusTypeDef set_adjustment_torque_floor(float adjustment_torque_floor_value)
{
	adjustment_torque_floor_value *= ADJUSTMENT_TORQUE_FLOOR_BASE;
	if(adjustment_torque_floor_value < 1 || adjustment_torque_floor_value > 30)
	{
		ERROR_PRINT("Failed to set adjustment_torque_floor value\n The range is [1, 30]\r\n");
		return HAL_ERROR;
	}
    adjustment_torque_floor = adjustment_torque_floor_value;
	DEBUG_PRINT("Setting adjustment_torque_floor to: %f, the adjustment_torque_floor is now: %f\r\n", adjustment_torque_floor_value, adjustment_torque_floor);
	return HAL_OK;
}

float get_tc_kP(void)
{
	DEBUG_PRINT("TC_kP: %f (default %f)\r\n", tc_kP, TC_kP_DEFAULT);
	return tc_kP;
}

float get_error_floor(void)
{
	return error_floor;
}

float get_adjustment_torque_floor(void)
{
	DEBUG_PRINT("Adjusment_torque_floor: %f (default %f)\r\n", adjustment_torque_floor, ADJUSTMENT_TORQUE_FLOOR_DEFAULT);
	return adjustment_torque_floor;
}
