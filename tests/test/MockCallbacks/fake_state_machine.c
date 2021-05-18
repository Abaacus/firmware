#include "fake_state_machine.h"

pthread_t fake_fsmInit(HAL_StatusTypeDef (*init_function)(void), FSM_Handle_Struct *handle) {
	init_function();
	fsm_run(handle);
}
