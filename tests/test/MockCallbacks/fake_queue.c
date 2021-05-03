#include "fake_queue.h"
#include "stddef.h"
#include "fake_hal_defs.h"
typedef struct MyQueue_t {
	size_t data_size;	

} MyQueue_t;

#define MAX_NUM_QUEUES 10
static MyQueue_t queue_data[MAX_NUM_QUEUES];

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition ){
	return HAL_OK;
}

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType ){
	return NULL;
}

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition )
{
	return HAL_OK;
}


BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait){
	return HAL_OK;
}


HAL_StatusTypeDef watchdogTaskCheckIn(uint32_t id){
	return HAL_OK;
}
