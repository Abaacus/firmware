/**
  *****************************************************************************
  * @file    processCAN.c
  * @author  Jacky Lim
  * @brief   Processes the RX CAN messages coming from the computer
  * @details Decodes each RX message and perform the corresponding action
  *****************************************************************************
  */

#include "processCAN.h"

#include "canReceive.h"
#include "userInit.h"

#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <stdio.h>

/* process_rx_task defines and variables */
#define TV_BTN_mask     (1 << TV_BTN_BIT)  
#define EM_BTN_mask     (1 << EM_BTN_BIT)
#define HV_BTN_mask     (1 << HV_BTN_BIT)
#define NAV_L_BTN_mask  (1 << NAV_L_BTN_BIT)
#define NAV_R_BTN_mask  (1 << NAV_R_BTN_BIT)
#define SEL_BTN_mask    (1 << SEL_BTN_BIT)
#define TC_BTN_mask     (1 << TC_BTN_BIT)

typedef struct {
    uint8_t DCU_Output_TV_BTN;
    uint8_t DCU_Output_EM_BTN;
    uint8_t DCU_Output_HV_BTN;
    uint8_t DCU_Output_NAV_L_BTN;
    uint8_t DCU_Output_NAV_R_BTN;
    uint8_t DCU_Output_SEL_BTN;
    uint8_t DCU_Output_TC_BTN;
} DCU_Output_t;

static uint8_t dByte0 = 0;
static DCU_Output_t buttons;

void set_output_pins(DCU_Output_t *buttons);

void process_rx_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1) {
        xQueueReceive(vcu_hil_queue, &can_msg, portMAX_DELAY);
        
        if (can_msg.extd) { // only parse messages in extended CAN frame
            switch (can_msg.identifier) {
                case DCU_HIL_OUTPUT_ID:  
                    dByte0 = can_msg.data[0];
                    buttons.DCU_Output_TV_BTN = (dByte0 & TV_BTN_mask);
                    buttons.DCU_Output_EM_BTN = (dByte0 & EM_BTN_mask);
                    buttons.DCU_Output_HV_BTN = (dByte0 & HV_BTN_mask);
                    buttons.DCU_Output_NAV_L_BTN = (dByte0 & NAV_L_BTN_mask);
                    buttons.DCU_Output_NAV_R_BTN = (dByte0 & NAV_R_BTN_mask);
                    buttons.DCU_Output_SEL_BTN = (dByte0 & SEL_BTN_mask);
                    buttons.DCU_Output_TC_BTN = (dByte0 & TC_BTN_mask);

                    set_output_pins(&buttons);
                default:
                    break;
            }
        } else {
            printf("Error: received non-extended CAN messages\n");
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(PROCESS_RX_TASK_INTERVAL_MS));
    }
}

// Defining a CAN message
twai_message_t CAN_output_status = 
{
    .identifier = DCU_HIL_OUTPUT_STATUS_ID,
    .extd = 1,
    .data_length_code = 1,
};

/* A helper function for setting the GPIO pins */
void set_output_pins(DCU_Output_t *buttons) {
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        if (gpio_set_level(OUTPUT_PIN_ARRAY[i], buttons->DCU_Output_TV_BTN + i) == ESP_OK) {
            CAN_output_status.data[i] = PIN_SET;
        }
    }

    twai_transmit(&CAN_output_status, portMAX_DELAY); // send CAN message
}