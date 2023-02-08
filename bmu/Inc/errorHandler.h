#ifndef ERRORHANDLER_H

#define ERRORHANDLER_H
#include "main.h"

typedef enum Error_events_t {
    Failed_debugInit = 0,
    Failed_uartStartReceiving,
    Failed_canInit,
    Failed_initBusVoltagesAndCurrentQueues,
    Failed_stateMachineMockInit,
    Failed_controlInit,
    Failed_initPackVoltageQueue,
    Failed_init_imd_measurement,
    Failed_init_HW_check_timer,
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
