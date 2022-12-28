#ifndef CANRECEIVE_H
#define CANRECEIVE_H

#include "debug.h"

typedef struct DTC_Received{
    int code;
    int severity;
    int data;
} DTC_Received;

typedef struct DTC_History{
    DTC_Received history[DTC_HISTORY_LENGTH];
    int count;
} DTC_History;

#endif /* end of include guard: CANRECEIVE_H */
