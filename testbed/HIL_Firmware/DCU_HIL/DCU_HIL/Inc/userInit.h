#ifndef USER_INIT_H_
#define USER_INIT_H_

#include "driver/gpio.h"

#define TASK_PRIORITY (configMAX_PRIORITIES - 1)

/* PINS */
// CAN
#define CAN_TX GPIO_NUM_12 //20
#define CAN_RX GPIO_NUM_13 //21

// Output pins
#define EM_BTN_PIN      GPIO_NUM_4
#define HV_BTN_PIN      GPIO_NUM_5
#define TC_BTN_PIN      GPIO_NUM_6
#define TV_BTN_PIN      GPIO_NUM_7
#define NAV_R_BTN_PIN   GPIO_NUM_39 //32
#define NAV_L_BTN_PIN   GPIO_NUM_40 //33
#define SEL_BTN_PIN     GPIO_NUM_41 //34

// Data is encoded in a byte where each bit correspond to a signal
typedef enum {
    TV_BTN_BIT,
    EM_BTN_BIT,
    HV_BTN_BIT,
    NAV_L_BTN_BIT,
    NAV_R_BTN_BIT,
    SEL_BTN_BIT,
    TC_BTN_BIT,
    OUTPUT_COUNT,
} Output_Bit_t;

// Input pins
#define MC_LED_PIN      GPIO_NUM_15 //8
#define IMD_LED_PIN     GPIO_NUM_16 //9
#define MOT_RED_PIN     GPIO_NUM_17 //10
#define MOT_GR_PIN      GPIO_NUM_18 //11
#define AMS_RED_PIN     GPIO_NUM_8 //12
#define AMS_GR_PIN      GPIO_NUM_3 //15
#define BUZZER_PIN      GPIO_NUM_46 //16
#define EM_LED_PIN      GPIO_NUM_35 //28
#define TV_LED_PIN      GPIO_NUM_36 //29
#define HV_LED_PIN      GPIO_NUM_37 //30
#define TC_LED_PIN      GPIO_NUM_38 //31

// Data is encoded in a byte where each bit correspond to a signal
typedef enum {
    AMS_GR_BIT,
    AMS_RED_BIT,
    BUZZER_BIT,
    EM_LED_BIT,
    HV_LED_BIT,
    IMD_LED_BIT,
    MC_LED_BIT,
    MOT_GR_BIT,
    MOT_RED_BIT,
    TC_LED_BIT,
    TV_LED_BIT,
    INPUT_COUNT,
} Input_Bit_t;

extern const int OUTPUT_PIN_ARRAY[];
extern const int INPUT_PIN_ARRAY[];

#endif /* USER_INIT_H_ */