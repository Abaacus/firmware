#include "endurance_mode.h"
#include "FreeRTOS.h"
#include "stm32f7xx_hal.h"
#include "stdbool.h"
#include "vcu_F7_can.h"
#include "cmsis_os.h"
#include "watchdog.h"
#include "motorController.h"
#include "debug.h"

#define ENDURANCE_MODE_TASK_ID 2
#define ENDURANCE_MODE_TASK_PERIOD (500)
#define ENDURANCE_MODE_FLAG_BIT (0)
// Reminder these are per motor
#define DISCHARGE_CURRENT_MAX_A (30.0f)
#define DISCHARGE_CURRENT_MIN_A (5.0f)
#define SATURATION_INTEGRAL_BUFFER (2.0f)
#define INITIAL_SOC_DEFAULT (0.0f)
#define NUM_LAPS_DEFAULT (0U)
#define EM_KP_DEFAULT (200.0f)
#define EM_KI_DEFAULT (0.2f)

static float initial_soc = INITIAL_SOC_DEFAULT;
static uint32_t num_laps = NUM_LAPS_DEFAULT;
static uint32_t num_laps_to_complete = NUMBER_OF_LAPS_TO_COMPLETE_DEFAULT*(ENDURANCE_MODE_BUFFER);
static bool in_endurance_mode = false;
static float em_kP = EM_KP_DEFAULT;
static float em_kI = EM_KI_DEFAULT;
extern osThreadId enduranceModeHandle;

void endurance_mode_EM_callback(void)
{
	static bool has_set_initial_soc = false;
	if(!has_set_initial_soc)
	{	
		initial_soc = StateBatteryChargeHV;
		has_set_initial_soc = true;
	}
}

void set_lap_limit(uint32_t laps)
{
	num_laps_to_complete = laps;
}

void trigger_lap(void)
{
	num_laps++;
	xTaskNotifyGive(enduranceModeHandle);
}

void toggle_endurance_mode(void)
{
	in_endurance_mode = !in_endurance_mode;
}

float clamp_motor_current(float current)
{
	if(current > DISCHARGE_CURRENT_MAX_A)
	{
		return DISCHARGE_CURRENT_MAX_A;
	}
	else if(current < DISCHARGE_CURRENT_MIN_A)
	{
		return DISCHARGE_CURRENT_MIN_A;
	}
	return current;
}

static HAL_StatusTypeDef compute_discharge_limit(float * current_limit)
{
	static float last_output_current = DISCHARGE_CURRENT_MAX_A;
	static float error_accum = 0.0f;

	float soc = StateBatteryChargeHV/100.0f;
	// Amount of SoC we should be at to end with 5%
	float expected_soc = (float)(num_laps_to_complete - num_laps)/(float)num_laps_to_complete*initial_soc; 
	float error = expected_soc - soc;
	
	// We want to prevent integral windup when in saturation region
	if(last_output_current < DISCHARGE_CURRENT_MAX_A - SATURATION_INTEGRAL_BUFFER
			&& last_output_current > DISCHARGE_CURRENT_MIN_A + SATURATION_INTEGRAL_BUFFER)
	{
		// integral error, dt = 1 as our time is 1 lap
		error_accum += error;
	}
	
	float output_current = last_output_current - (error*em_kP + error_accum*em_kI);
	*current_limit = clamp_motor_current(output_current);	
	last_output_current = *current_limit;
	return HAL_OK;
}

void enduranceModeTask(void *pvParameters)
{
	if (registerTaskToWatch(ENDURANCE_MODE_TASK_ID, 2*pdMS_TO_TICKS(ENDURANCE_MODE_TASK_PERIOD), false, NULL) != HAL_OK)
	{
		ERROR_PRINT("ERROR: Failed to init endurance mode task, suspending endurance mode task\n");
		while(1);
	}
	while(1)
	{
		uint32_t wait_flag = ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS(ENDURANCE_MODE_TASK_PERIOD/2));
		if(wait_flag & (1U << ENDURANCE_MODE_FLAG_BIT))
		{
			float current_limit = 0.0f;
			// We are in endurance mode
			if(in_endurance_mode)
			{
				HAL_StatusTypeDef ret = compute_discharge_limit(&current_limit);
				if(ret != HAL_OK)
				{
					ERROR_PRINT("Failed to compute discharge limit for endurance mode\n");
					current_limit = DISCHARGE_CURRENT_LIMIT_DEFAULT;
				}
			}
			else
			{
				current_limit = DISCHARGE_CURRENT_LIMIT_DEFAULT;
			}
			setDischargeCurrentLimit(current_limit);
		}
		else
		{
			// The flag was never actually set, we just hit the timeout	
		}
		watchdogTaskCheckIn(ENDURANCE_MODE_TASK_ID);
		vTaskDelay(ENDURANCE_MODE_TASK_PERIOD);
	}

}

HAL_StatusTypeDef set_initial_soc(float initial_soc_value)
{
	if(initial_soc_value < 0 || initial_soc_value > 1)
	{
		ERROR_PRINT("Failed to set initial_soc\nValue should be from 0 to 1\n");
		return HAL_ERROR;
	}
	initial_soc = initial_soc_value;
	return HAL_OK;
}

HAL_StatusTypeDef set_num_laps(uint8_t num_laps_value)
{
	if(num_laps_value < 0 || num_laps_value > num_laps_to_complete)
	{
		ERROR_PRINT("Failed to set num_laps\nValue should be from 0 to %lu\n", num_laps_to_complete);
		return HAL_ERROR;
	}
	num_laps = num_laps_value;
	return HAL_OK;
}

void set_num_laps_complete(uint32_t num_laps_complete_value)
{
	num_laps_to_complete = num_laps_complete_value;
}

void set_in_endurance_mode(bool in_endurance_mode_bool)
{
	in_endurance_mode = in_endurance_mode_bool;
}

void set_em_kP(float em_kP_value)
{
	em_kP = em_kP_value;
}

void set_em_kI(float em_kI_value)
{
	em_kI = em_kI_value;
}

float get_initial_soc(void)
{
	DEBUG_PRINT("Initial_soc: %f (default %f)\n", initial_soc, INITIAL_SOC_DEFAULT);
	return initial_soc;
}

uint32_t get_num_laps(void)
{
	DEBUG_PRINT("Num_laps: %lu (default %u)\n", num_laps, NUM_LAPS_DEFAULT);
	return num_laps;
}

uint32_t get_num_laps_complete(void)
{
	DEBUG_PRINT("Num_laps_complete: %lu (default %u)\n", num_laps_to_complete, NUMBER_OF_LAPS_TO_COMPLETE_DEFAULT);
	return num_laps_to_complete;
}

bool get_in_endurance_mode(void)
{
	DEBUG_PRINT("In_endurance_mode: %d (default false)\n", in_endurance_mode);
	return in_endurance_mode;
}

float get_em_kP(void)
{
	DEBUG_PRINT("Em_kP: %f (default %f)\n", em_kP, EM_KP_DEFAULT);
	return em_kP;
}

float get_em_kI(void)
{
	DEBUG_PRINT("Em_kI: %f (default %f)\n", em_kI, EM_KI_DEFAULT);
	return em_kI;
}