#include "fake_tasks.h"
#include <pthread.h>

pthread_t start_task(void *(*task_function)(void *), void* args) {
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, task_function, args); 
	return thread_id;
}

void end_task(pthread_t thread_id) {
	pthread_cancel(thread_id);
}

typedef enum
{
	eNoAction = 0,				/* Notify the task without updating its notify value. */
	eSetBits,					/* Set bits in the task's notification value. */
	eIncrement,					/* Increment the task's notification value. */
	eSetValueWithOverwrite,		/* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
	eSetValueWithoutOverwrite	/* Set the task's notification value if the previous value has been read by the task. */
} eNotifyAction;
BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) {

	return 0;
}
void vTaskDelay( const TickType_t xTicksToDelay ) {
	// I don't think we need to do anything here
	// If we actually put this here, it will cause tests to take along time
	// It will also not fully emulate STM chip
}

TickType_t xTaskGetTickCountFromISR(void) {
	return 0;
}
// Real tick implementations should be created
TickType_t xTaskGetTickCount( void ) {
	return 0;
}

BaseType_t xTaskGetSchedulerState( void ) {
	return taskSCHEDULER_RUNNING;
}
