#ifndef ERRORHANDLER_H

#define ERRORHANDLER_H
#include "main.h"

typedef enum Error_events_t 
{
    Failed_debugInit = 0,                                 // 0
    Failed_uartStartReceiving,                            // 1
    Failed_canInit,                                       // 2
    Failed_initBusVoltagesAndCurrentQueues,               // 3
    Failed_stateMachineMockInit,                          // 4
    Failed_controlInit,                                   // 5
    Failed_initPackVoltageQueue,                          // 6
    Failed_init_imd_measurement,                          // 7
    Failed_init_HW_check_timer,                           // 8
    Failed_ADC_Init,                                      // 9
    Failed_ADC_ConfigChannel,                             // 10
    Failed_DMA_Init,                                      // 11
    Failed_CAN_Init,                                      // 12
    Failed_IWDG_Init,                                     // 13
    Failed_RCC_OscConfig,                                 // 14
    Failed_PWREx_EnableOverDrive,                         // 15
    Failed_RCC_ClockConfig,                               // 16   
    Failed_RCCEx_PeriphCLKConfig,                         // 17
    Failed_SPI_Init,                                      // 18
    Failed_TIM_Base_Init,                                 // 19
    Failed_TIM_ConfigClockSource,                         // 20
    Failed_TIM_PWM_Init,                                  // 21
    Failed_TIMEx_MasterConfigSynchronization,             // 22
    Failed_TIM_PWM_ConfigChannel,                         // 23
    Failed_TIM_IC_Init,                                   // 24
    Failed_TIM_SlaveConfigSynchro,                        // 25
    Failed_TIM_IC_ConfigChannel,                          // 26
    Failed_UART_Init,                                     // 27
    Failed_HV_Measure_Task_Unregistered_with_Watchdog,    // 28
    Failed_IMD_Measurement_not_Started,                   // 29
    Failed_IMD_Task_Unregistered_with_Watchdog,           // 30
    Failed_initVoltageAndTempArrays,                      // 31
    Failed_Battery_Task_Unregistered_with_Watchdog,       // 32
    Failed_canStart,                                      // 33
    Failed_Fault_Monitor_Task_Unregistered_with_Watchdog, // 34
    Failed_pcdcInit,                                      // 35
    Failed_Brake_Sensor_ADC_DMA_Conversions_not_Started,  // 36
    Failed_sensorTaskInit,                                // 37
    Failed_fanInit                                        // 38
} Error_events_t;

void BMU_error(uint8_t error_num);

#if IS_BOARD_NUCLEO_F7
#ifndef Error_Handler
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#endif
#endif

// Runtime assert
#define c_assert(e) ((e) ? (0) : log_assert_violation(__FILE__,__LINE__,#e))
int log_assert_violation(char *file, int line, char *condition);

#endif /* end of include guard: ERRORHANDLER_H */
