#ifndef CAN_RECEIVE_COMMON_H

#define CAN_RECEIVE_COMMON_H

#include "bsp.h"
#include "debug.h"

// Code value for an empty DTC_Received
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

    // Index of the next dtc to be added to the array
    uint8_t tail;   // incremented after each insertion, reset to 0 if it reaches DTC_HISTORY_LENGTH
} DTC_History_t;

void CAN_Msg_UartOverCanTx_Callback();
#endif /* end of include guard: CAN_RECEIVE_COMMON_H */
