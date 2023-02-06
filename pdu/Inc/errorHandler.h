typedef enum pdu_error_enum {
     Failure_ADC_Init = 0,
     Failure_ADC_ConfigChannel,
     Failure_DMA_Init,
     Failure_CAN_Init,
     Failure_IWDG_Init,
     Failure_RCC_OscConfig,
     Failure_PWREx_EnableOverDrive,
     Failure_RCC_ClockConfig,
     Failure_RCCEx_PeriphCLKConfig,
     Failure_TIM_Base_Init,
     Failure_TIM_ConfigClockSource,
     Failure_TIMEx_MasterConfigSynchronization,
     Failure_UART_Init,
     Failure_registerTaskToWatch,
     Failure_debugInit,
     Failure_initStateMachines,
     Failure_canInit,
     Failure_mockStateMachineInit,
     Failure_ADC_Start_DMA,
     Failure_TIM_Base_Start,
     Failure_startADCConversions,
     Failure_canStart
} pdu_error_enum;

void PDU_error(pdu_error_enum error_num);