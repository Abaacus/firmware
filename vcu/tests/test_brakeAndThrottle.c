#include "unity.h"
#include "adc_units.h"
#include "brakeAndThrottle.h"

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

#include "Mock_userCan.h"
#include "pdu_can.h"
#include "Mock_canHeartbeat.h"

#include "debug.h"
#include "motorController.h"
#include "vcu_F7_can.h"
#include "vcu_F7_dtc.h"

typedef void* ADC_HandleTypeDef;

void setUp(void) {
}

void tearDown(void) {
}

void test_map_range(){
	TEST_ASSERT_TRUE(map_range(1800, 1740, 1895, 0, 100) == 1200/31); //in is in current range
	TEST_ASSERT_TRUE(map_range(1700, 1740, 1895, 0, 100) == 0); //in is lower than current range
	TEST_ASSERT_TRUE(map_range(1900, 1740, 1895, 0, 100) == 100); //in is higher than current range
	TEST_ASSERT_TRUE(map_range(1800, 1740, 1895, 50, 100) == 2150/31); //in current range, non-zero low_out	
	TEST_ASSERT_TRUE(map_range(1700, 1740, 1895, 50, 100) == 50); //in is lower than current range, non-zero low_out
	TEST_ASSERT_TRUE(map_range(1900, 1740, 1895, 50, 100) == 100); // in is higher than current range, non-zero low_out
}

void test_getBrakePositionPercent(){
	brakeThrottleSteeringADCVals[BRAKE_POS_INDEX] = 1800; //in range
	TEST_ASSERT_TRUE(getBrakePositionPercent() == (1200/31));
	brakeThrottleSteeringADCVals[BRAKE_POS_INDEX] = 1700; //below range
	TEST_ASSERT_TRUE(getBrakePositionPercent() == 0);
	brakeThrottleSteeringADCVals[BRAKE_POS_INDEX] = 1900; //above range
	TEST_ASSERT_TRUE(getBrakePositionPercent() == 100);
	brakeThrottleSteeringADCVals[BRAKE_POS_INDEX] = 1740; //at low range
	TEST_ASSERT_TRUE(getBrakePositionPercent() == 0);
	brakeThrottleSteeringADCVals[BRAKE_POS_INDEX] = 1895; //at high range
	TEST_ASSERT_TRUE(getBrakePositionPercent() == 100);
}

void test_is_throttle1_in_range(){i
	TEST_ASSERT_TRUE(!(is_throttle1_in_range((THROTT_A_LOW - 1)))); //below low range
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_LOW)); //at low range
	TEST_ASSERT_TRUE(is_throttle1_in_range((THROTT_A_LOW + THROTT_A_HIGH)/2)); //in range
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_HIGH)); //no deadzone max throttle
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_HIGH + MAX_THROTTLE_A_DEADZONE)); //at max, including deadzone
	TEST_ASSERT_TRUE(!(is_throttle1_in_range(THROTT_A_HIGH + MAX_THROTTLE_A_DEADZONE + 1))); //past max throttle, including deadzone
}

void test_is_throttle2_in_range(){
	TEST_ASSERT_TRUE(!(is_throttle2_in_range(THROTT_B_LOW - MAX_THROTTLE_B_DEADZONE - 1))); //below low minus deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW - MAX_THROTT_B_DEADZONE)); //at lowest point, including deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW - (MAX_THROTT_B_DEADZONE/2))); //within deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW)); //at low point, not including deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range((THROTT_B_HIGH + (THROTT_B_LOW - MAX_THROTTLE_B_DEADZONE))/2); //within range
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_HIGH)); //at max point
	TEST_ASSERT_TRUE(!(is_throttle2_in_range(THROTT_B_HIGH + 10))); // past max point
}

void test_calculate_throttle_percent1(){
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_LOW - 10) == 100); //below low
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_LOW) == 100); //at low
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_LOW + 1) == 100 - (100 * 1/(THROTT_A_HIGH - THROTT_A_LOW))); //in range
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_HIGH) == 0); //at max
	TEST_ASSERT_TRUE(is_throttle1_in_range(THROTT_A_HIGH + 10) == 0); //above max
}

void test_calculate_throttle_percent2(){
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW - 10) == 0); //below low
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW) == 0); //at low
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_LOW + 1) == 100 * 1/(THROTT_B_HIGH + THROTT_B_LOW)); //in range
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_HIGH) == 100); //at max
	TEST_ASSERT_TRUE(is_throttle2_in_range(THROTT_B_HIGH + 10) == 100); //above max
}
