#include "errorHandler.h"
#include "debug.h"
#include "pdu_dtc.h"

void PDU_error(Error_events_t error_num){
     ERROR_PRINT("PDU ERROR: %u", error_num);
     sendDTC_CRITICAL_PDU_FAILURE(error_num);
}