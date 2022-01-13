#include "unity.h"
 
#include "controlStateMachine.h"
 
#include "Mock_debug.h"
#include "fake_debug.h"


#include "queue.h"

#include "main.h"

#include "state_machine.h"
#include "fake_state_machine_interface.h"

#include "task.h"

#include "Mock_watchdog.h"

#include "tim.h"
#include "can.h"

#include "gpio.h"

#include "Mock_userCan.h"
#include "pdu_can.h"

#include "Mock_canHeartbeat.h"

 
extern HAL_StatusTypeDef motorControlInit(void);
extern HAL_StatusTypeDef coolingControlInit(void);
extern HAL_StatusTypeDef maincontrolInit(void);
extern HAL_StatusTypeDef turnBoardsOn(void);
extern HAL_StatusTypeDef turnBoardsOff(void);
extern HAL_StatusTypeDef startLVCuttoffDelay(void);
 
void setUp(void)
{
    fake_mock_init_queues();
    fake_mock_init_debug();
 
}

void test_sample_delete_before_merge()
{
	TEST_ASSERT_TRUE(1 == 1);
}

void test_motorControlInit()
{
   TEST_ASSERT_TRUE(motorControlInit() == HAL_OK);
   TEST_ASSERT_TRUE(fsmGetState(&motorFsmHandle) == MTR_STATE_Motors_Off);
}

void test_coolingControlInit()
{
    TEST_ASSERT_TRUE(coolingControlInit() == HAL_OK);
    TEST_ASSERT_TRUE(fsmGetState(&coolingFsmHandle) == COOL_STATE_OFF);
}


void test_maincontrolInit()
{
    TEST_ASSERT_TRUE(maincontrolInit() == HAL_OK);
    TEST_ASSERT_TRUE(fsmGetState(&mainFsmHandle) == MN_STATE_Boards_Off);
}

void test_initStateMachines()
{
    TEST_ASSERT_TRUE(initStateMachines() == HAL_OK);
}
/*
void test_startCriticalFailureDelay()
{
    TEST_ASSERT_TRUE(startCriticalFailureDelay() == MN_STATE_Warning_Critical);
}
*/
void test_startLVCuttoffDelay() 
{
    TEST_ASSERT_TRUE(startLVCuttoffDelay() == MN_STATE_LV_Shutting_Down);
}

void test_turnBoardsOn() 
{
    TEST_ASSERT_TRUE(turnBoardsOn() == HAL_OK);
}

void test_turnBoardsOff() 
{
    TEST_ASSERT_TRUE(turnBoardsOff() == HAL_OK);
}

void tearDown(void)
{
}
