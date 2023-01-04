#ifndef CANRECEIVE_H
#define CANRECEIVE_H

#include "debug.h"

typedef struct DTC_Received {
    int code;
    int severity;
    int data;
} DTC_Received_t;

typedef struct DTC_History {
    DTC_Received_t dtcs[DTC_HISTORY_LENGTH];
    int tail;
} DTC_History_t;

void DTC_History_init();
DTC_History_t * get_DTC_History();

#endif /* end of include guard: CANRECEIVE_H */
