#include "unity.h"
 
#include "controlStateMachine.h"
 
#include "Mock_debug.h"
#include "fake_debug.h"


#include "queue.h"

#include "main.h"

#include "Mock_state_machine.h"
#include "fake_state_machine_interface.h"

#include "task.h"

#include "Mock_watchdog.h"

#include "tim.h"
#include "can.h"
#include "gpio.h"

#include "Mock_userCan.h"
#include "Mock_BMU_can.h"

 
 
void setUp(void)
{
    fake_mock_init_queues();
    fake_mock_init_debug();
 
}

void test_sample_delete_before_merge()
{
	TEST_ASSERT_TRUE(1 == 1);
}

void tearDown(void)
{
}
