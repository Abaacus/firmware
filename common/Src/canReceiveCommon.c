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

// DTC Logging for the printDTCs CLI command
void CAN_Receive_Log_DTC(int DTC_CODE, int DTC_Severity, int DTC_Data, DTC_History_t * DTC_Log) {
    if (DTC_Severity == DTC_Severity_FATAL 
        || DTC_Severity == DTC_Severity_CRITICAL) {
    
        DTC_Log->dtcs[DTC_Log->tail].code = DTC_CODE;
        DTC_Log->dtcs[DTC_Log->tail].severity = DTC_Severity;
        DTC_Log->dtcs[DTC_Log->tail].data = DTC_Data;
        DTC_Log->tail = (DTC_Log->tail + 1) % DTC_HISTORY_LENGTH;
    }    
}

