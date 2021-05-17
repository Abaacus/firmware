#ifndef FAKE_TASKS_H
#define FAKE_TASKS_H
#include <pthread.h>
#include <stdint.h>
#include "fake_timer.h"


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

#endif
