//Includes
#include "unity.h"
#include "state_machine.h"
#include <pthread.h>
#include "fake_debug.h"
#include "fake_queue.h"
#include "fake_main.h"
#include "Mock_main.h"
#include "fake_state_machine_interface.h"
#include "fake_tasks.h"
#include <unistd.h>

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
	init_queues();
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
	size_t transitionTableLength;
	
	uint32_t eventQueueLength = 5;
	uint32_t watchdogTaskId = 0;
	
	uint32_t startingState = 0;

	switch(data_num){
		case(1):
			state_any = STATE_1_ANY;
			event_any = EV_1_ANY;
			transitions = transitions_1;
			transitionTableLength = TRANS_COUNT(transitions_1);
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
    init.transitionTableLength = transitionTableLength;
    init.eventQueueLength = eventQueueLength;
    init.watchdogTaskId = watchdogTaskId;
    if (fsmInit(startingState, &init, handle) != HAL_OK) {
		TEST_MESSAGE("Failure initializing FSM");
		return HAL_ERROR;
    }
    return HAL_OK;


}

#define FSM_START(id) \
	FSM_Handle_Struct fsmHandle_##id; \
	if(HAL_OK != setup_fsm_handle(id, &fsmHandle_##id)){TEST_ASSERT_TRUE(0);} \
	pthread_t fsm_handle_thread_id_##id = fsm_run(&fsmHandle_##id);

#define FSM_END(id) \
	end_task(fsm_handle_thread_id_##id);


#define POLL_TIME_US 20 * 1000 // 100ms
#define POLL_DURATION_US 1 * 1000 * 1000 // 5s
#define POLL_COND(cond) { \
	uint32_t count = 0; \
	while(!(cond) && count < POLL_DURATION_US / POLL_TIME_US){ \
		/*fprintf(stderr, "Poll\n");*/ \
		usleep(POLL_TIME_US); \
		count++; \
	} \
	if(count >= POLL_DURATION_US / POLL_TIME_US){TEST_MESSAGE("Failed Poll");}\
}

/* Ensure FSM initialization works */
void test_Init_Close(void)
{
  	FSM_START(1);
    TEST_ASSERT_TRUE(1);
    FSM_END(1);
}

/* Test at least one transition works, STATE_1_1 -> STATE_1_2 */
void test_One_Transition(void)
{
  	FSM_START(1);
	
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_1);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_1);

	fsmSendEvent(&fsmHandle_1, EV_1_1, 0);
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_2);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_2);

	FSM_END(1);
}


/* Basically just go through FSM 1, STATE_1_1 -> STATE_1_4 */
void test_Multiple_Transition(void)
{
  	FSM_START(1);
	
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_1);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_1);

	fsmSendEvent(&fsmHandle_1, EV_1_1, 0);
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_2);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_2);

	fsmSendEvent(&fsmHandle_1, EV_1_2, 0);
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_3);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_3);
	
	fsmSendEvent(&fsmHandle_1, EV_1_3, 0);
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_4);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_4);
	TEST_ASSERT_FALSE(fsmGetState(&fsmHandle_1) == STATE_1_3);
	TEST_ASSERT_FALSE(fsmGetState(&fsmHandle_1) == STATE_1_2);
	TEST_ASSERT_FALSE(fsmGetState(&fsmHandle_1) == STATE_1_1);
	
	FSM_END(1);
}

/* This is a stupid test that will always pass, I leave it in for future extension
 * Basically whenever a state transition doesn't exist, we just do not change states
 * But how do we check this? Because the test could run before event is processed. We need more robust tests.*/

void test_Fail_Transition(void)
{
	FSM_START(1);
	
	fsmSendEvent(&fsmHandle_1, EV_1_4, 0);
	POLL_COND(fsmGetState(&fsmHandle_1) == STATE_1_1);
	TEST_ASSERT_TRUE(fsmGetState(&fsmHandle_1) == STATE_1_1);
	
	FSM_END(1);
}


/* To be implemented */
void test_Multiple_FSMs(void){
	TEST_ASSERT_TRUE(1);
}

/* Add test for queue testing, basically send a bunch of messages */
