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
