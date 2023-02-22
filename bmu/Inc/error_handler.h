#ifndef ERROR_HANDLER_H

#define ERROR_HANDLER_H
#include "main.h"

typedef enum
{
    Failed_debugInit = 0,                                 // 0
    Failed_uartStartReceiving,                            // 1
    Failed_canInit,                                       // 2
    Failed_initBusVoltagesAndCurrentQueues,               // 3
    Failed_stateMachineMockInit,                          // 4
    Failed_controlInit,                                   // 5
    Failed_initPackVoltageQueue,                          // 6
    Failed_initIMDmeasurement,                            // 7
    Failed_initHWcheckTimer,                              // 8
    Failed_HVmeasureTaskUnregisteredWithWatchdog,         // 9
    Failed_IMDmeasurementNotStarted,                      // 10
    Failed_IMDtaskUnregisteredWithWatchdog,               // 11
    Failed_initVoltageAndTempArrays,                      // 12
    Failed_batteryTaskUnregisteredWithWatchdog,           // 13
    Failed_canStart,                                      // 14
    Failed_faultMonitorTaskUnregisteredWithWatchdog,      // 15
    Failed_pcdcInit,                                      // 16
    Failed_brakeSensorADCtoDMAconversionsNotStarted,      // 17
    Failed_sensorTaskInit,                                // 18
    Failed_fanInit                                        // 19
} Error_events;

void BMU_error(Error_events error_num);

#if IS_BOARD_NUCLEO_F7
#ifndef Error_Handler
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#endif
#endif

// Runtime assert
#define c_assert(e) ((e) ? (0) : log_assert_violation(__FILE__,__LINE__,#e))
int log_assert_violation(char *file, int line, char *condition);

#endif /* end of include guard: ERROR_HANDLER_H */
