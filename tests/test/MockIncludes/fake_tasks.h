#ifndef FAKE_TASKS_H
#define FAKE_TASKS_H
#include <pthread.h>
#include <stdint.h>
#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif


// Start a thread with a task function
pthread_t start_task(void *(*task_function)(void *), void* args);

// End thread given pthread_t
void end_task(pthread_t thread_id);
void vTaskDelay( const TickType_t xTicksToDelay );
#endif
