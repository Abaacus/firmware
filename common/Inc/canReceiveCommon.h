#ifndef CAN_RECEIVE_COMMON_H

#define CAN_RECEIVE_COMMON_H

#include "bsp.h"
#include "debug.h"

// Code value for an empty DTC_Received since no DTCs have a code of -1
#define EMPTY_DTC_ENTRY -1

// Length of the DTC history stored, viewed with the command printDTCs
#define DTC_HISTORY_LENGTH 10

typedef struct DTC_Received {
    int16_t code;
    uint8_t severity;
    uint64_t data;
} DTC_Received_t;

typedef struct DTC_History {
    DTC_Received_t dtcs[DTC_HISTORY_LENGTH];

    uint8_t tail;  // Index in dtcs where the next dtc will be added, in range [0, DTC_HISTORY_LENGTH) 
} DTC_History_t;

void DTC_History_init();
DTC_History_t * get_DTC_History();

void CAN_Msg_UartOverCanTx_Callback();
void CAN_Receive_Log_DTC(int16_t DTC_CODE, uint8_t DTC_Severity, uint64_t DTC_Data);
#endif /* end of include guard: CAN_RECEIVE_COMMON_H */
