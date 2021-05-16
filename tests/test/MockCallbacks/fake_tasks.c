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

}
