#include "fake_queue.h"
#include "FreeRTOS.h"
#include "stddef.h"
#include "fake_hal_defs.h"
#include "projdefs.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "queue.h"

#define queueUNLOCKED					( ( int8_t ) -1 )
#define queueLOCKED_UNMODIFIED			( ( int8_t ) 0 )
typedef int List_t;
typedef struct QueueDefinition
{
	int8_t *pcHead;					/*< Points to the beginning of the queue storage area. */
	int8_t *pcTail;					/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */
	int8_t *pcWriteTo;				/*< Points to the free next place in the storage area. */

	union							/* Use of a union is an exception to the coding standard to ensure two mutually exclusive structure members don't appear simultaneously (wasting RAM). */
	{
		int8_t *pcReadFrom;			/*< Points to the last place that a queued item was read from when the structure is used as a queue. */
		UBaseType_t uxRecursiveCallCount;/*< Maintains a count of the number of times a recursive mutex has been recursively 'taken' when the structure is used as a mutex. */
	} u;

	List_t xTasksWaitingToSend;		/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
	List_t xTasksWaitingToReceive;	/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

	volatile UBaseType_t uxMessagesWaiting;/*< The number of items currently in the queue. */
	UBaseType_t uxLength;			/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
	UBaseType_t uxItemSize;			/*< The size of each items that the queue will hold. */

	volatile int8_t cRxLock;		/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
	volatile int8_t cTxLock;		/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t ucStaticallyAllocated;	/*< Set to pdTRUE if the memory used by the queue was statically allocated to ensure no attempt is made to free the memory. */
	#endif

	#if ( configUSE_QUEUE_SETS == 1 )
		struct QueueDefinition *pxQueueSetContainer;
	#endif

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t uxQueueNumber;
		uint8_t ucQueueType;
	#endif

} xQUEUE;

/* The old xQUEUE name is maintained above then typedefed to the new Queue_t
name below to enable the use of older kernel aware debuggers. */
typedef xQUEUE Queue_t;
static BaseType_t prvCopyDataToQueue( Queue_t * const pxQueue, const void *pvItemToQueue, const BaseType_t xPosition );
static void prvCopyDataFromQueue( Queue_t * const pxQueue, void * const pvBuffer );

#define MAX_NUM_QUEUES 10
#define MAX_QUEUE_LENGTH 1024
static int8_t queue_data[MAX_NUM_QUEUES][MAX_QUEUE_LENGTH];
static Queue_t queues[MAX_NUM_QUEUES];
static uint8_t num_queues_used = 0;

/*
 * TODO: Lists, tasks should be implemented
 * */
BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition ){
	Queue_t * const pxQueue = ( Queue_t * ) xQueue;
	// Wait until we have space in the queue
//	while( false == (( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) || ( xCopyPosition == queueOVERWRITE )) ){}
	
	if( ( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) || ( xCopyPosition == queueOVERWRITE ) ) {
		prvCopyDataToQueue(pxQueue, pvItemToQueue, xCopyPosition);	
		return HAL_OK;
	}
	return HAL_ERROR;
}

// For testing I will always statically allocate queues just as a way to check for memory leaks in production
// 
QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType ){
	TEST_ASSERT_TRUE_MESSAGE(num_queues_used < MAX_NUM_QUEUES, "Too many queues allocated in the code. \
																This could be due to an error where too many queues are initialized(memory leak) \
																or perhaps you just have to increment MAX_NUM_QUEUES");	

	TEST_ASSERT_TRUE_MESSAGE(uxItemSize * uxQueueLength < MAX_QUEUE_LENGTH, "Too large space allocated to queue."
																			  "Increment MAX_QUEUE_LENGTH, or perhaps the queue is too long");

	Queue_t new_queue = {
		.pcHead = queue_data[num_queues_used],
		.pcTail = &queue_data[num_queues_used][uxItemSize * uxQueueLength],
		.uxLength = uxQueueLength,
		.uxItemSize = uxItemSize,
	};

	xQueueGenericReset(&new_queue, 0);

	queues[num_queues_used] = new_queue;	

	return &(queues[num_queues_used++]);
}

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition )
{
	Queue_t * const pxQueue = ( Queue_t * ) xQueue;
	if( ( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) || ( xCopyPosition == queueOVERWRITE ) ) {
		prvCopyDataToQueue(pxQueue, pvItemToQueue, xCopyPosition);	
		return HAL_OK;
	}
	return HAL_ERROR;
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
	Queue_t * const pxQueue = ( Queue_t * ) xQueue;
	const UBaseType_t uxMessagesWaiting = pxQueue->uxMessagesWaiting;

	if( uxMessagesWaiting > ( UBaseType_t ) 0 )
	{
		prvCopyDataFromQueue( pxQueue, pvBuffer );
		pxQueue->uxMessagesWaiting = uxMessagesWaiting - ( UBaseType_t ) 1;
	}
	return pdTRUE;
}




BaseType_t xQueueGenericReset( QueueHandle_t xQueue, BaseType_t xNewQueue )
{
	Queue_t * const pxQueue = ( Queue_t * ) xQueue;
	pxQueue->pcTail = pxQueue->pcHead + ( pxQueue->uxLength * pxQueue->uxItemSize );
	pxQueue->uxMessagesWaiting = ( UBaseType_t ) 0U;
	pxQueue->pcWriteTo = pxQueue->pcHead;
	pxQueue->u.pcReadFrom = pxQueue->pcHead + ( ( pxQueue->uxLength - ( UBaseType_t ) 1U ) * pxQueue->uxItemSize );
	pxQueue->cRxLock = queueUNLOCKED;
	pxQueue->cTxLock = queueUNLOCKED;
	return 0;
}



static BaseType_t prvCopyDataToQueue( Queue_t * const pxQueue, const void *pvItemToQueue, const BaseType_t xPosition ) {
	BaseType_t xReturn = pdFALSE;
	BaseType_t uxMessagesWaiting = pxQueue->uxMessagesWaiting;
	if(xPosition == queueSEND_TO_BACK) {
		//
		( void ) memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); 
		pxQueue->pcWriteTo += pxQueue->uxItemSize;
		
		if( pxQueue->pcWriteTo >= pxQueue->pcTail )	{
			pxQueue->pcWriteTo = pxQueue->pcHead;
		}

	} else {
		( void ) memcpy( ( void * ) pxQueue->u.pcReadFrom, pvItemToQueue, ( size_t ) pxQueue->uxItemSize ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
		pxQueue->u.pcReadFrom -= pxQueue->uxItemSize;
		if( pxQueue->u.pcReadFrom < pxQueue->pcHead ) /*lint !e946 MISRA exception justified as comparison of pointers is the cleanest solution. */
		{
			pxQueue->u.pcReadFrom = ( pxQueue->pcTail - pxQueue->uxItemSize );
		}

		if( xPosition == queueOVERWRITE )
		{
			if( uxMessagesWaiting > ( UBaseType_t ) 0 )
			{
				/* An item is not being added but overwritten, so subtract
				one from the recorded number of items in the queue so when
				one is added again below the number of recorded items remains
				correct. */
				--uxMessagesWaiting;
			}
		}
	}

	pxQueue->uxMessagesWaiting = uxMessagesWaiting + ( UBaseType_t ) 1;
	return xReturn;
}


static void prvCopyDataFromQueue( Queue_t * const pxQueue, void * const pvBuffer )
{
	if( pxQueue->uxItemSize != ( UBaseType_t ) 0 )
	{
		pxQueue->u.pcReadFrom += pxQueue->uxItemSize;
		if( pxQueue->u.pcReadFrom >= pxQueue->pcTail ) {
			pxQueue->u.pcReadFrom = pxQueue->pcHead;
		}
		( void ) memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->u.pcReadFrom, ( size_t ) pxQueue->uxItemSize ); 	
	}
}


void fake_mock_init_queues() {
	num_queues_used = 0;
	for(int queue = 0;queue < MAX_NUM_QUEUES;queue++) {
		queues[queue] = (Queue_t){0};
		for(int q_byte = 0; q_byte < MAX_QUEUE_LENGTH; q_byte++) {
			queue_data[queue][q_byte] = 0;
		}
	}
}
