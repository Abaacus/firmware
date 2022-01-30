#include "unity.h"
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

#include "gpio.h"

#include "Mock_userCan.h"
#include "pdu_can.h"
#include "Mock_canHeartbeat.h"

#include "debug.h"
#include "motorController.h"
#include "vcu_F7_can.h"
#include "vcu_F7_dtc.h"

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

void test_is_throttle1_in_range(){
	TEST_ASSERT_TRUE(!(is_throttle1_in_range(2199))); //below low range
	TEST_ASSERT_TRUE(is_throttle1_in_range(2100)); //at low range
	TEST_ASSERT_TRUE(is_throttle1_in_range(2200)); //in range
	TEST_ASSERT_TRUE(is_throttle1_in_range(2325)); //no deadzone max throttle
	TEST_ASSERT_TRUE(is_throttle1_in_range(2500)); //including deadzone
	TEST_ASSERT_TRUE(is_throttle1_in_range(2625)); //at max, including deadzone
	TEST_ASSERT_TRUE(!(is_throttle1_in_range(2626))); //past max throttle, including deadzone
}

void test_is_throttle2_in_range(){
	TEST_ASSERT_TRUE(!(is_throttle2_in_range(1824))); //below low minus deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(1825)); //at lowest point, including deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(1900)); //within deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(2125)); //at low point, not including deadzone
	TEST_ASSERT_TRUE(is_throttle2_in_range(2340)); //within range
	TEST_ASSERT_TRUE(is_throttle2_in_range(2350)); //at max point
	TEST_ASSERT_TRUE(!(is_throttle2_in_range(2360))); // past max point
}

void test_calculate_throttle_percent1(){
	TEST_ASSERT_TRUE(is_throttle1_in_range(2000) == 100); //below low
	TEST_ASSERT_TRUE(is_throttle1_in_range(2100) == 100); //at low
	TEST_ASSERT_TRUE(is_throttle1_in_range(2145) == (80)); //in range
	TEST_ASSERT_TRUE(is_throttle1_in_range(2325) == 0); //at max
	TEST_ASSERT_TRUE(is_throttle1_in_range(2400) == 0); //above max
}

void test_calculate_throttle_percent2(){
	TEST_ASSERT_TRUE(is_throttle2_in_range(2000) == 0); //below low
	TEST_ASSERT_TRUE(is_throttle2_in_range(2125) == 0); //at low
	TEST_ASSERT_TRUE(is_throttle2_in_range(2170) == 20); //in range
	TEST_ASSERT_TRUE(is_throttle2_in_range(2350) == 100); //at max
	TEST_ASSERT_TRUE(is_throttle2_in_range(2400) == 100); //above max
}
