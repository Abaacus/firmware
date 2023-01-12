#include "vcu_errorHandler.h"
#include "vcu_F7_dtc.h"
#include "debug.h"


ErrorMes_t errorMes = 0;
void error(uint16_t error_num){
    errorMes = error_num;
    ERROR_PRINT("VCU ERROR: %d\n", errorMes);
    sendDTC_CRITICAL_VCU_F7_ERROR(error_num);
}