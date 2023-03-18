#include "canReceiveCommon.h"

#include "userCan.h"
#include "bsp.h"
#include "debug.h"
#include "boardTypes.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#if BOARD_ID  == ID_BMU
	#include "bmu_can.h"
	#include "bmu_dtc.h"
#elif BOARD_ID  == ID_DCU
	#include "dcu_can.h"
	#include "dcu_dtc.h"
#elif BOARD_ID  == ID_VCU_F7
	#include "vcu_F7_can.h"
	#include "vcu_F7_dtc.h"
#elif BOARD_ID  == ID_PDU
	#include "pdu_can.h"
	#include "pdu_dtc.h"
#elif BOARD_ID  == ID_WSBFL
	#include "wsbfl_can.h"
	#include "wsbfl_dtc.h"
#elif BOARD_ID  == ID_WSBFR
	#include "wsbfr_can.h"
	#include "wsbfr_dtc.h"
#elif BOARD_ID  == ID_WSBRL
	#include "wsbrl_can.h"
	#include "wsbrl_dtc.h"
#elif BOARD_ID  == ID_WSBRR
	#include "wsbrr_can.h"
	#include "wsbrr_dtc.h"
#endif

#if IS_BOARD_F7
static DTC_History_t DTC_Log;

void DTC_History_init() {
    DTC_Log.tail = 0;
    for (uint8_t i = 0; i < DTC_HISTORY_LENGTH; i++) {
        DTC_Log.dtcs[i].code = EMPTY_DTC_ENTRY;
    }
}

DTC_History_t * get_DTC_History() {
    return &DTC_Log;
}

// DTC Logging for the printDTCs CLI command
void CAN_Receive_Log_DTC(int16_t DTC_Code, uint8_t DTC_Severity, int64_t DTC_Data) {
    if (DTC_Severity == DTC_Severity_FATAL  ||
		DTC_Severity == DTC_Severity_CRITICAL) {
        DTC_Log.dtcs[DTC_Log.tail].code = DTC_Code;
        DTC_Log.dtcs[DTC_Log.tail].severity = DTC_Severity;
        DTC_Log.dtcs[DTC_Log.tail].data = DTC_Data;
        DTC_Log.tail = (DTC_Log.tail + 1) % DTC_HISTORY_LENGTH;
    }
}
#endif

void CAN_Msg_UartOverCanTx_Callback() 
{
	if (!isUartOverCanEnabled)
	{
		return;
	}
	uint8_t data = (uint8_t) UartOverCanTX;
	BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR( uartRxQueue, &data, &xHigherPriorityTaskWoken );

    if( xHigherPriorityTaskWoken )
    {
        portYIELD();
    }
}

#if (BOARD_ID == ID_BMU || BOARD_ID == ID_PDU)
void CAN_Msg_VCU_F7_DTC_Callback(int16_t DTC_Code, uint8_t DTC_Severity, int64_t DTC_Data) {
    CAN_Receive_Log_DTC(DTC_Code, DTC_Severity, DTC_Data);
}
#endif

#if (BOARD_ID == ID_PDU || BOARD_ID == ID_VCU_F7 || BOARD_ID == ID_BMU)
void CAN_Msg_DCU_DTC_Callback(int16_t DTC_Code, uint8_t DTC_Severity, int64_t DTC_Data) {
    CAN_Receive_Log_DTC(DTC_Code, DTC_Severity, DTC_Data);
}

void CAN_Msg_ChargeCart_DTC_Callback(int16_t DTC_Code, uint8_t DTC_Severity, int64_t DTC_Data) {
    CAN_Receive_Log_DTC(DTC_Code, DTC_Severity, DTC_Data);
}
#endif

