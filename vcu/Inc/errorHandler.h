#ifndef ERRORHANDLER_H

#define ERRORHANDLER_H
#include "stm32f7xx_hal.h"
#include "stdio.h"
#include "main.h"
#include "FreeRTOS.h"
#include "vcu_F7_dtc.h"
#include "bsp.h"
#include "debug.h"

typedef enum Error_events_t {

    Failed_ADC_conversions = 0,                 /// <0: Failed to start ADC DMA conversions in brakeAndThrottle.c
    Failed_Brake_Throttle_timer,                /// <1: Failed to start brake and throttle adc timer in brakeAndThrottle.c
    Failed_driveByWireTask,                     /// <2: Failed to complete drive by wire task in drive_by_wire.c
    Failed_debugInit,                           /// <3: Failed to initialize debug in userInit.c
    Failed_driveByWireInit,                     /// <4: Failed to initialize drive by wire in userInit.c
    Failed_canInit,                             /// <5: Failed to initialize CAN in userInit.c
    Failed_stateMachineMockInit,                /// <6: Failed to initialize State Machine Mock in userInit.c
    Failed_initMotorControllerProcanSettings,   /// <7: Failed to initialize Motor controller setting in userInit.c
    Failed_beagleboneOff,                       /// <8: Failed Beaglebone off function in in userInit.c
} Error_events_t;

void VCU_error(uint8_t error_num);

#endif /* end of include guard: ERRORHANDLER_H */