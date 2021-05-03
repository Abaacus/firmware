//Includes
#include "unity.h"
#include "state_machine.h"
#include <pthread.h>
#include "fake_debug.h"
#include "fake_queue.h"
#include "fake_main.h"
#include "Mock_main.h"

// FSM common Data

FSM_Handle_Struct fsmHandle;


// FSM Data 1

typedef enum States_1_t {
    STATE_1_1,
    STATE_1_2,
    STATE_1_3,
    STATE_1_4,
    STATE_1_ERROR,
    STATE_1_ANY,
} States_1_t;

typedef enum Events_1_t {
    EV_1_1,
    EV_1_2,
    EV_1_3,
    EV_1_4,
    EV_1_5,
    EV_1_6,
    EV_1_ANY,
} Events_1_t;


// Transitions Functions


uint32_t transition_1_1(uint32_t event)
{
	switch(event){
		case(EV_1_1):
			return STATE_1_2;
		case(EV_1_2):
			return STATE_1_3;
		case(EV_1_3):
			return STATE_1_4;
	}
    return STATE_1_ERROR;
}




// Transitions
Transition_t transitions_1[] = {
    {STATE_1_1,        EV_1_1,         &transition_1_1},
    {STATE_1_2,        EV_1_2,         &transition_1_1},
    {STATE_1_3,        EV_1_3,         &transition_1_1},
    {STATE_1_4,        EV_1_4,         &transition_1_1},
};


// Unity Standard Functions
void setUp(void)
{
	_Error_Handler_Stub(fake_Error_Handler);
	debugInit();
}

void tearDown(void)
{
}



// Utils Functions
HAL_StatusTypeDef setup_fsm_handle(uint8_t data_num, FSM_Handle_Struct *handle){
	uint32_t state_any;
	uint32_t event_any;
	Transition_t *transitions;
	size_t events_size;
	
	uint32_t eventQueueLength = 5;
	uint32_t watchdogTaskId = 0;
	
	uint32_t startingState = 0;

	switch(data_num){
		case(1):
			state_any = STATE_1_ANY;
			event_any = EV_1_ANY;
			transitions = transitions_1;
			events_size = sizeof(Events_1_t); 
			break;
		default:
			return HAL_ERROR;
	}

	
	FSM_Init_Struct init;
    init.maxStateNum = state_any;
    init.maxEventNum = event_any;
    init.sizeofEventEnumType = events_size;
    init.ST_ANY = state_any;
    init.EV_ANY = event_any;
    init.transitions = transitions;
    init.transitionTableLength = TRANS_COUNT(transitions);
    init.eventQueueLength = eventQueueLength;
    init.watchdogTaskId = watchdogTaskId;
    if (fsmInit(startingState, &init, handle) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;


}

void *my_test_fsm_task(void *args){
	fsmTaskFunction(args);
}

#define FSM_START(id) \
	FSM_Handle_Struct fsmHandle_##id; \
	setup_fsm_handle(id, &fsmHandle_##id); \
	pthread_t thread_id_##id; \
	pthread_create(&thread_id_##id, NULL, my_test_fsm_task, (void*)&fsmHandle_##id);  

#define FSM_END(id) \
	pthread_cancel(thread_id_##id);
void test_Standard_Transitions(void)
{
  	FSM_START(1);
    TEST_ASSERT_TRUE(1);
    FSM_END(1);
//	Error_Handler();
}
void test_Fail_Transition(void)
{
}
