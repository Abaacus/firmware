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
    Failed_ADC_Init,
    Failed_ADC_ConfigChannel,
    Failed_DMA_Init,
    Failed_CAN_Init,
    Failed_IWDG_Init,
    Failed_RCC_OscConfig,
    Failed_PWREx_EnableOverDrive,
    Failed_RCC_ClockConfig,
    Failed_RCCEx_PeriphCLKConfig,
    Failed_SPI_Init,
    Failed_TIM_Base_Init,
    Failed_TIM_ConfigClockSource,
    Failed_TIM_PWM_Init,
    Failed_TIMEx_MasterConfigSynchronization,
    Failed_TIM_IC_ConfigChannel,
    Failed_TIM_PWM_ConfigChannel,
    Failed_TIM_IC_Init,
    Failed_TIM_SlaveConfigSynchro,
    Failed_TIM_IC_ConfigChannel,
    Failed_UART_Init,
    Failed_HV_Measure_Task_Unregistered_with_Watchdog,
    Failed_IMD_Measurement_not_Started,
    Failed_IMD_Task_Unregistered_with_Watchdog,
    Failed_initVoltageAndTempArrays,
    Failed_Battery_Task_Unregistered_with_Watchdog,
    Failed_canStart,
    Failed_Fault_Monitor_Task_Unregistered_with_Watchdog,
    Failed_pcdcInit,
    Failed_Brake_Sensor_ADC_DMA_Conversions_not_Started,
    Failed_sensorTaskInit,
    Failed_fanInit
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
