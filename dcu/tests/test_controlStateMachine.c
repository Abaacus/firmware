#include "unity.h"

#include "controlStateMachine.h"

#include "Mock_stm32f0xx_hal.h"
#include "Mock_userCan.h"
#include "Mock_DCU_can.h"

#include "Mock_debug.h"
#include "fake_debug.h"

#include "fake_queue.h"

#include "Mock_main.h"
#include "fake_main.h"

#include "state_machine.h"

#include "fake_state_machine_interface.h"

#include "Mock_canReceive.h"

#include "Mock_can.h"
#include "fake_can.h"

#include "fake_timer.h"
#include "fake_tasks.h"

#include "Mock_gpio.h"
#include "fake_gpio.h"

#include "Mock_watchdog.h"

// These CAN variables have to be initialized for use in our tests
volatile int64_t ButtonEMEnabled;
volatile int64_t ButtonHVEnabled;



void setUp(void)
{
	fake_mock_init_queues();
	_Error_Handler_Stub(fake_Error_Handler);
	fake_mock_init_debug();

}

void tearDown(void)
{
}

void test_simple_fsm(void)
{
	
	TEST_ASSERT_TRUE(dcuFsmInit() == HAL_OK);
	pthread_t fsm_id = fake_mock_fsm_run(&DCUFsmHandle);
	
	// 
	sendCAN_DCU_buttonEvents_ExpectAndReturn(HAL_OK);
	fsmSendEvent(&DCUFsmHandle, EV_HV_Toggle, 0);
	fake_mock_wait_for_fsm_state(&DCUFsmHandle, STATE_HV_Toggle);
	TEST_ASSERT_TRUE(ButtonHVEnabled == 1);



	end_task(fsm_id);
}


