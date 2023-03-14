#ifndef MOTOR_COOLING_H
#define MOTOR_COOLING_H
#include "stm32f7xx_hal.h"
#include "stdbool.h"

#define COOLING_TASK_INTERVAL_MS 5000 //5 seconds
#define COOLING_START_THRESHOLD_C 30 //Cooling system on at 30 degrees C
#define HYSTERESIS_C 5 //Allows cooling to turn on at 30C but turn off at 25C to prevent constant turning on and off of cooling system
#define COOLING_STOP_THRESHOLD_C (COOLING_START_THRESHOLD_C - HYSTERESIS_C) //Cooling system off at 25 degrees C
#define COOLING_TASK_ID 6

extern float tempMotorRightSum;
extern uint32_t numTempSamplesRight;
extern float averageTempRight;
 
extern float tempMotorLeftSum;
extern uint32_t numTempSamplesLeft;
extern float averageTempLeft;

#endif /* end of include guard: MOTOR_COOLING_H */