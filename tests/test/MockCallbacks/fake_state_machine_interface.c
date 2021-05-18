#include "fake_state_machine_interface.h"
#include "fake_tasks.h"

void *my_test_fsm_task(void *args){
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	fsmTaskFunction(args);
}
pthread_t fsm_run(FSM_Handle_Struct *handle) {
	return start_task(my_test_fsm_task, handle);
}
