#include "unity.h"

#include "controlStateMachine.h"

#include "Mock_userCan.h"

#include "fake_debug.h"

#include "can.h"
#include "dcu_can.h"
#include "tim.h"
#include "timers.h"
#include "main.h"
#include "queue.h"
#include "gpio.h"
#include "task.h"

#include "state_machine.h"

#include "fake_state_machine_interface.h"

#include "Mock_canReceive.h"


#include "Mock_watchdog.h"
#include "Mock_canHeartbeat.h"

#include "gpio_ports.h"



void setUp(void)
{
	fake_mock_init_queues();
	fake_mock_init_debug();
}

void tearDown(void)
{
}

void test_simple_fsm(void)
{

}

void test_motorControlInit(void)
{
	// TEST_ASSERT_TRUE(motorControlInit() == HAL_OK);
	// MotorControlInit_ExpectAndReturn(HAL_OK); 
    // TEST_ASSERT_TRUE(tfsmGetState(&motorFsmHandle) == MTR_STATE_Motors_Off);
	fsmInit_ExpectAndReturn(MTR_STATE_Motors_Off, &init, &motorFsmHandle, HAL_OK);
	registerTaskToWatch_ExpectAndReturn(3, 50, true, &motorFsmHandle, HAL_OK);

    // motorControlInit();
}





