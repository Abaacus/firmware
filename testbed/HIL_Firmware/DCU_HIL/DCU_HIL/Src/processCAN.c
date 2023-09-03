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
#define TV_BTN_MASK     (1 << TV_BTN_BIT)  
#define EM_BTN_MASK     (1 << EM_BTN_BIT)
#define HV_BTN_MASK     (1 << HV_BTN_BIT)
#define NAV_L_BTN_MASK  (1 << NAV_L_BTN_BIT)
#define NAV_R_BTN_MASK  (1 << NAV_R_BTN_BIT)
#define SEL_BTN_MASK    (1 << SEL_BTN_BIT)
#define TC_BTN_MASK     (1 << TC_BTN_BIT)
#define IMD_LED_MASK    (1 << IMD_LED_BIT) // end of first byte
#define BUZZER_MASK     (1 << (BUZZER_BIT - BITS_IN_BYTE))
#define MC_LED_MASK     (1 << (MC_LED_BIT - BITS_IN_BYTE))

typedef struct {
    bool DCU_Output_TV_BTN;
    bool DCU_Output_EM_BTN;
    bool DCU_Output_HV_BTN;
    bool DCU_Output_NAV_L_BTN;
    bool DCU_Output_NAV_R_BTN;
    bool DCU_Output_SEL_BTN;
    bool DCU_Output_TC_BTN;
    bool DCU_Output_IMD_LED;
    bool DCU_Output_BUZZER;
    bool DCU_Output_MC_LED;
} DCU_Output_t;

static uint8_t dByte0 = 0;
static uint8_t dByte1 = 0;
static DCU_Output_t output;

void set_output_pins(DCU_Output_t *output);

void process_rx_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1) {
        xQueueReceive(dcu_hil_queue, &can_msg, portMAX_DELAY);
        
        if (can_msg.extd) { // only parse messages in extended CAN frame
            switch (can_msg.identifier) {
                case DCU_HIL_OUTPUT_ID:  
                    dByte0 = can_msg.data[0];
                    dByte1 = can_msg.data[1];

                    output.DCU_Output_TV_BTN = (dByte0 & TV_BTN_MASK);
                    output.DCU_Output_EM_BTN = (dByte0 & EM_BTN_MASK);
                    output.DCU_Output_HV_BTN = (dByte0 & HV_BTN_MASK);
                    output.DCU_Output_NAV_L_BTN = (dByte0 & NAV_L_BTN_MASK);
                    output.DCU_Output_NAV_R_BTN = (dByte0 & NAV_R_BTN_MASK);
                    output.DCU_Output_SEL_BTN = (dByte0 & SEL_BTN_MASK);
                    output.DCU_Output_TC_BTN = (dByte0 & TC_BTN_MASK);
                    output.DCU_Output_IMD_LED = ((dByte0 & IMD_LED_MASK) ? 0 : 1); // invert signal
                    output.DCU_Output_BUZZER = ((dByte1 & BUZZER_MASK) ? 0 : 1); // invert signal
                    output.DCU_Output_MC_LED = ((dByte1 & MC_LED_MASK) ? 0 : 1); // invert signal

                    set_output_pins(&output);
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
    .data_length_code = 2,
};

/* A helper function for setting the GPIO pins */
void set_output_pins(DCU_Output_t *output) {
    uint8_t dByte0 = 0;
    uint8_t dByte1 = 0;

    for (int i = 0; i < OUTPUT_COUNT; i++) {
        bool *level = (bool *)output + i; // iterate over elements

        if (gpio_set_level(OUTPUT_PIN_ARRAY[i], *level) == ESP_OK) {
            if (i < BITS_IN_BYTE) { // 1st byte
                dByte0 |= (PIN_SET << i);
            } else {
                dByte1 |= (PIN_SET << (i - BITS_IN_BYTE)); // 2nd byte
            }
        }
    }

    CAN_output_status.data[0] = dByte0;
    CAN_output_status.data[1] = dByte1;
    twai_transmit(&CAN_output_status, portMAX_DELAY); // send CAN message
}