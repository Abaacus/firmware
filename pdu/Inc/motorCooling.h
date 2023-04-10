#ifndef MOTOR_COOLING_H
#define MOTOR_COOLING_H
#include "stm32f7xx_hal.h"
#include "stdbool.h"

#define COOLING_TASK_INTERVAL_MS 5000 // 5 seconds
#define COOLING_TIMER_DELAY_MS 1000 // 1 second
#define FATAL_FAILURE_DELAY_MS 1000 // 1 second
#define LV_CUTTOFF_DELAY_MS 1000 // 1 second
#define COOLING_START_THRESHOLD_C 30 // Cooling system on at 30 degrees C
#define COOLING_THRESHOLD_BUFFER_C 5 // Allows cooling to turn on at 30C but turn off at 25C to prevent constant turning on and off of cooling system
#define COOLING_STOP_THRESHOLD_C (COOLING_START_THRESHOLD_C - COOLING_THRESHOLD_BUFFER_C) // Cooling system off at 25 degrees C
#define COOLING_TASK_ID 6

extern float tempMotorRightSum;
extern uint32_t numMotorTempSamplesRight;
extern float averageTempMotorRight;
 
extern float tempMotorLeftSum;
extern uint32_t numMotorTempSamplesLeft;
extern float averageTempMotorLeft;

typedef enum coolingEvents{
    COOLING_EVENT_DELAY_ELAPSED = 0,
    COOLING_EVENT_DISABLE,
    COOLING_EVENT_LV_CUTOFF,
    COOLING_EVENT_FATAL_DTC,
}coolingEvents;

void coolingSystemOff(void);
void coolingSystemOn(void);
void sendCoolingEvent(coolingEvents event);

#endif /* MOTOR_COOLING_H */