#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "stdint.h"

void error(uint16_t error_num);

typedef enum ErrorMes_t{
    ADC_DMA_conversions_Failure =1,
    BrakeAndTrottle_adc_Timer_Failure=2,
    Drive_By_Wire_CanStart_Failure=3,
    Init_DeBug_Failure=4,
    Init_driveByWire_Failure=5,
    Init_Can_Failure=6,
    Init_stateMachineMock_Failure = 7,
    Init_MotorControllerProcanSettings_Failure=8,
    beagleboneOff_Failed=9
}ErrorMes_t;


#endif