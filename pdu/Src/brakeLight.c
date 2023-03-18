#include "brakeLight.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pdu_can.h"
#include "debug.h"

static float brake_light_on_threshold = 15;

#define BRAKE_LIGHT_ON_THRESHOLD (brake_light_on_threshold)
#define BRAKE_TASK_PERIOD_MS 300

bool isBrakePressed(uint32_t brakePercent)
{
    return (BrakePercent > BRAKE_LIGHT_ON_THRESHOLD);
}

void CAN_Msg_VCU_Data_Callback()
{
    if (isBrakePressed(BrakePercent)) {
        BRAKE_LIGHT_ENABLE;
    } else {
        BRAKE_LIGHT_DISABLE;
    }
}

void set_brake_light_on_threshold(float brake_light_on_threshold_value)
{
    brake_light_on_threshold = brake_light_on_threshold_value;
}
