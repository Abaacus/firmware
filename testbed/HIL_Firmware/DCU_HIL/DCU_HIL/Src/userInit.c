/**
  *****************************************************************************
  * @file    userInit.c
  * @author  Jacky Lim
  * @brief   Initializes peripherals and task registration
  * @details Setups CAN (TWAI), GPIO for input and output, and register task
  *****************************************************************************
  */

#include "userInit.h"

#include "canReceive.h"
#include "processCAN.h"
#include "processInput.h"

#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_err.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#include <stdio.h>

void create_task(void)
{
    BaseType_t xReturned;
    TaskHandle_t CAN_rx_task_handler; // found in common
    TaskHandle_t CAN_process_task_handler;
    TaskHandle_t poll_DCU_output_task_handler;

    xReturned = xTaskCreate(
        can_rx_task,
        "CAN_RECEIVE_TASK",
        4096,
        NULL,
        TASK_PRIORITY,
        &CAN_rx_task_handler
    );

    if(xReturned != pdPASS) {
        printf("Failed to register can_rx_task to FreeRTOS");
    }

    xReturned = xTaskCreate(
        process_rx_task,
        "CAN_PROCESS_TASK",
        4096,
        NULL,
        TASK_PRIORITY,
        &CAN_process_task_handler
    );

    if(xReturned != pdPASS) {
        printf("Failed to register process_rx_task to FreeRTOS");
    }

    xReturned = xTaskCreate(
        poll_DCU_output_task,
        "POLL_DCU_OUTPUT_TASK",
        4096,
        NULL,
        TASK_PRIORITY,
        &poll_DCU_output_task_handler
    );

    if(xReturned != pdPASS) {
        printf("Failed to register poll_DCU_output_task to FreeRTOS");
    }
}

esp_err_t CAN_init(void)
{
    // Configure driver
    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = CAN_TX,
        .rx_io = CAN_RX,
        .clkout_io = TWAI_IO_UNUSED, 
        .bus_off_io = TWAI_IO_UNUSED,      
        .tx_queue_len = MAX_QUEUE_LENGTH, 
        .rx_queue_len = MAX_QUEUE_LENGTH,                          
        .alerts_enabled = TWAI_ALERT_NONE,  
        .clkout_divider = 0,        
        .intr_flags = ESP_INTR_FLAG_LEVEL1
    };
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        printf("Driver installed\n");
    } else {
        printf("Failed to install driver\n");
        return ESP_FAIL;
    }

    //Start TWAI driver
    if (twai_start() == ESP_OK) {
        printf("Driver started\n");
        return ESP_OK;
    } else {
        printf("Failed to start driver\n");
        return ESP_FAIL;
    }
}

/* Pin arrays */
// Output array
const int OUTPUT_PIN_ARRAY[] = {
    TV_BTN_PIN, EM_BTN_PIN, HV_BTN_PIN, NAV_L_BTN_PIN,
    NAV_R_BTN_PIN, SEL_BTN_PIN, TC_BTN_PIN
};

// Input array
const int INPUT_PIN_ARRAY[] = {
    AMS_GR_PIN, AMS_RED_PIN, BUZZER_PIN, EM_LED_PIN, HV_LED_PIN, 
    IMD_LED_PIN, MC_LED_PIN, MOT_GR_PIN, MOT_RED_PIN, TC_LED_PIN,
    TV_LED_PIN
};

void GPIO_init(void)
{
    // Input pins
    for (int i = 0; i < INPUT_COUNT; i++) {
        gpio_set_direction(INPUT_PIN_ARRAY[i], GPIO_MODE_INPUT);
    }

    // Output pins
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        gpio_set_direction(OUTPUT_PIN_ARRAY[i], GPIO_MODE_OUTPUT);
    }
}

void app_main(void)
{
    CAN_init();
    GPIO_init();
    create_task();
}