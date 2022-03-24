#ifndef FAKE_TASKS_H
#define FAKE_TASKS_H
#include <pthread.h>
#include <stdint.h>
#include "tim.h"


typedef long BaseType_t;

#define taskSCHEDULER_SUSPENDED		( ( BaseType_t ) 0 )
#define taskSCHEDULER_NOT_STARTED	( ( BaseType_t ) 1 )
#define taskSCHEDULER_RUNNING		( ( BaseType_t ) 2 )

// Start a thread with a task function
pthread_t start_task(void *(*task_function)(void *), void* args);

// End thread given pthread_t
void end_task(pthread_t thread_id);


void vTaskDelay( const TickType_t xTicksToDelay );


BaseType_t xTaskGetSchedulerState( void );
typedef void* TaskHandle_t;
typedef TaskHandle_t osThreadId;
typedef enum
{
	eNoAction = 0,				/* Notify the task without updating its notify value. */
	eSetBits,					/* Set bits in the task's notification value. */
	eIncrement,					/* Increment the task's notification value. */
	eSetValueWithOverwrite,		/* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
	eSetValueWithoutOverwrite	/* Set the task's notification value if the previous value has been read by the task. */
} eNotifyAction;
#define xTaskNotify( xTaskToNotify, ulValue, eAction )

#endif
