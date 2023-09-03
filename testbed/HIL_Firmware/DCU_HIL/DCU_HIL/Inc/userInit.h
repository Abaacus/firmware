#ifndef USER_INIT_H_
#define USER_INIT_H_

#include "driver/gpio.h"

#define TASK_PRIORITY (configMAX_PRIORITIES - 1)

/* PINS */
// CAN
#define CAN_TX GPIO_NUM_12
#define CAN_RX GPIO_NUM_13

// Output pins
#define EM_BTN_PIN      GPIO_NUM_4
#define HV_BTN_PIN      GPIO_NUM_5
#define TC_BTN_PIN      GPIO_NUM_6
#define TV_BTN_PIN      GPIO_NUM_7
#define NAV_R_BTN_PIN   GPIO_NUM_39
#define NAV_L_BTN_PIN   GPIO_NUM_40
#define SEL_BTN_PIN     GPIO_NUM_41
#define MC_LED_PIN      GPIO_NUM_15
#define IMD_LED_PIN     GPIO_NUM_16
#define BUZZER_PIN      GPIO_NUM_46


// Data is encoded in two bytes where each bit correspond to a signal
typedef enum {
    TV_BTN_BIT,
    EM_BTN_BIT,
    HV_BTN_BIT,
    NAV_L_BTN_BIT,
    NAV_R_BTN_BIT,
    SEL_BTN_BIT,
    TC_BTN_BIT,
    IMD_LED_BIT,
    BUZZER_BIT,
    MC_LED_BIT,
    OUTPUT_COUNT,
} Output_Bit_t;

// Input pins
#define MOT_RED_PIN     GPIO_NUM_17
#define MOT_GR_PIN      GPIO_NUM_18
#define AMS_RED_PIN     GPIO_NUM_8
#define AMS_GR_PIN      GPIO_NUM_3
#define EM_LED_PIN      GPIO_NUM_35
#define TV_LED_PIN      GPIO_NUM_36
#define HV_LED_PIN      GPIO_NUM_37
#define TC_LED_PIN      GPIO_NUM_38

// Data is encoded in a byte where each bit correspond to a signal
typedef enum {
    AMS_GR_BIT,
    AMS_RED_BIT,
    EM_LED_BIT,
    HV_LED_BIT,
    MOT_GR_BIT,
    MOT_RED_BIT,
    TC_LED_BIT,
    TV_LED_BIT,
    INPUT_COUNT,
} Input_Bit_t;

extern const int OUTPUT_PIN_ARRAY[];
extern const int INPUT_PIN_ARRAY[];

#endif /* USER_INIT_H_ */