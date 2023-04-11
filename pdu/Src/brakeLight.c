#include "brakeLight.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pdu_can.h"
#include "debug.h"

#define BRAKE_LIGHT_ON_THRESHOLD_DEFAULT (15.0f)
#define BRAKE_TASK_PERIOD_MS 300

static float brake_light_on_threshold = BRAKE_LIGHT_ON_THRESHOLD_DEFAULT;

bool isBrakePressed(uint32_t brakePercent)
{
    return (BrakePercent > brake_light_on_threshold);
}

void CAN_Msg_VCU_Data_Callback()
{
    if (isBrakePressed(BrakePercent)) {
        BRAKE_LIGHT_ENABLE;
    } else {
        BRAKE_LIGHT_DISABLE;
    }
}

#define BRAKE_LIGHT_ON_THRESHOLD_BASE (0.001f)

HAL_StatusTypeDef setBrakeLightOnThreshold(float brake_light_on_threshold_unit)
{
    brake_light_on_threshold_unit = brake_light_on_threshold_unit * BRAKE_LIGHT_ON_THRESHOLD_BASE;
    if(brake_light_on_threshold_unit < 0 || brake_light_on_threshold_unit > 100)
	{
		ERROR_PRINT("New brake light on threshhold value out of range. Should be [0, 100]\r\n");
		return HAL_ERROR;
	}
    brake_light_on_threshold = brake_light_on_threshold_unit;
    DEBUG_PRINT("Setting brake_light_on_threshold to: %f, the brake_light_on_threshold is now: %f\r\n", 
                brake_light_on_threshold_unit, brake_light_on_threshold);
	return HAL_OK;
}

float getBrakeLightOnThreshold(void)
{
    return brake_light_on_threshold;
}
