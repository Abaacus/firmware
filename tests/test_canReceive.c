#include "unity.h"
#include "canReceive.h"
#include "bsp.h"
#include <assert.h>
#include "VCU_F7_can.h"
#include "stdint.h"

uint64_t HV_Power_state, EM_State, DTC_CODE, FATAL_IMD_Failure, CRITICAL_CELL_VOLTAGE_LOW;

void CAN_Msg_BMU_DTC_Callback(int DTC_CODE, int DTC_Severity, int DTC_Data);

void setUp() {}

void tearDown() {}

void test_getHVState()
{
    HV_Power_state = 32;
    assert(getHVState() == HV_Power_State);
}

void test_getEMState()
{
    EM_State = 30;
    assert(getEMState() == EM_State);
}

/*
#define IMD_FAIL_LED_ON HAL_GPIO_WritePin(IMD_LED_EN_GPIO_Port, IMD_LED_EN_Pin, GPIO_PIN_SET);
#define AMS_FAIL_LED_ON HAL_GPIO_WritePin(AMS_LED_RED_EN_GPIO_Port, AMS_LED_RED_EN_Pin, GPIO_PIN_SET);
*/

void test_CAN_Msg_BMU_DTC_Callback()
{
    DTC_CODE = 0;
    CAN_Msg_BMU_DTC_Callback(0, 0, 0);

    FATAL_IMD_Failure = 0;
    assert(HAL_GPIO_ReadPin(IMD_LED_EN_GPIO_Port, IMD_LED_EN_Pin)==GPIO_PIN_SET);

    CRITICAL_CELL_VOLTAGE_LOW = 0;
    assert(HAL_GPIO_ReadPin(AMS_LED_RED_EN_GPIO_Port, AMS_LED_RED_EN_Pin)==GPIO_PIN_SET);
}
