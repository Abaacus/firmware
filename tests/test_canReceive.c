#include "unity.h"


#include "canReceive.h"

#include "bsp.h"
#include <assert.h>
#include "stdint.h"

#include "Mock_stm32f0xx_hal.h"
#include "Mock_userCan.h"

#include "Mock_can.h"
#include "fake_can.h"

#include "DCU_can.h"

#include "state_machine.h"

#include "fake_state_machine_interface.h"

#include "Mock_debug.h"
#include "fake_debug.h"

#include "fake_queue.h"

#include "Mock_main.h"
#include "fake_main.h"


#include "fake_timer.h"
#include "fake_tasks.h"

#include "Mock_gpio.h"
#include "fake_gpio.h"

#include "Mock_watchdog.h"

#include "Mock_controlStateMachine.h"

#include "Mock_canHeartbeat.h"

FSM_Handle_Struct DCUFsmHandle;


HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *sFilterConfig) {
	return fake_HAL_CAN_ConfigFilter(hcan, sFilterConfig);
}

void setUp() {
}

void tearDown() {}

void test_getHVState()
{
	// These tests are failing because getHVState is a boolean
    HV_Power_State = 32;
    TEST_ASSERT_TRUE(getHVState() == HV_Power_State);
}

void test_getEMState()
{
    EM_State = 30;
    TEST_ASSERT_TRUE(getEMState() == EM_State);
}

/*
#define IMD_FAIL_LED_ON HAL_GPIO_WritePin(IMD_LED_EN_GPIO_Port, IMD_LED_EN_Pin, GPIO_PIN_SET);
#define AMS_FAIL_LED_ON HAL_GPIO_WritePin(AMS_LED_RED_EN_GPIO_Port, AMS_LED_RED_EN_Pin, GPIO_PIN_SET);
*/

void test_CAN_Msg_BMU_DTC_Callback()
{
    DTC_CODE = 0;
    CAN_Msg_BMU_DTC_Callback(0, 0, 0);

	// BTW don't check GPIO pins in your tests
	// GPIO pins will not change as tests are being run on your own machine
	/*
    FATAL_IMD_Failure = 0;
    assert(HAL_GPIO_ReadPin(IMD_LED_EN_GPIO_Port, IMD_LED_EN_Pin)==GPIO_PIN_SET);

    CRITICAL_CELL_VOLTAGE_LOW = 0;
    assert(HAL_GPIO_ReadPin(AMS_LED_RED_EN_GPIO_Port, AMS_LED_RED_EN_Pin)==GPIO_PIN_SET);
	*/
	
}
