#ifndef FAKE_QUEUE_H
#define FAKE_QUEUE_H
#include "FreeRTOS.h"
#include "queue.h"

// This queue implementation must always be used
// You cannot stub queue functions, because CMock fails to parse queue.h

void fake_mock_init_queues();
#endif
