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
    Failed_HV_Measure_Task_Unregistered_with_Watchdog,    // 9
    Failed_IMD_Measurement_not_Started,                   // 10
    Failed_IMD_Task_Unregistered_with_Watchdog,           // 11
    Failed_initVoltageAndTempArrays,                      // 12
    Failed_Battery_Task_Unregistered_with_Watchdog,       // 13
    Failed_canStart,                                      // 14
    Failed_Fault_Monitor_Task_Unregistered_with_Watchdog, // 15
    Failed_pcdcInit,                                      // 16
    Failed_Brake_Sensor_ADC_DMA_Conversions_not_Started,  // 17
    Failed_sensorTaskInit,                                // 18
    Failed_fanInit                                        // 19
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
