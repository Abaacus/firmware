/**
  *****************************************************************************
  * @file    processInput.c
  * @author  Jacky Lim
  * @brief   Poll the output of the DCU board
  * @details The input of the DCU HIL is the output of the DCU board
  *****************************************************************************
  */

#include "processInput.h"

#include "userInit.h"

#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

// Defining a CAN message
twai_message_t CAN_DCU_HIL_input = 
{
    .identifier = DCU_HIL_INPUT_ID,
    .extd = 1,
    .data_length_code = 1,
};

/* Poll the output of the DCU board @ 1 ms */
void poll_DCU_output_task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint16_t DCU_GPIO_data = 0; 
	uint16_t byte_mask = 0xFF;

    while (1) {
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[0]) << AMS_GR_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[1]) << AMS_RED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[2]) << BUZZER_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[3]) << EM_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[4]) << HV_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[5]) << IMD_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[6]) << MC_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[7]) << MOT_GR_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[8]) << MOT_RED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[9]) << TC_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[10]) << TV_LED_BIT;

		// Extracting 1st and 2nd byte
		CAN_DCU_HIL_input.data[0] = (uint8_t)(DCU_GPIO_data & byte_mask);
		CAN_DCU_HIL_input.data[1] = (uint8_t)((DCU_GPIO_data >> 8) & byte_mask);
		
		twai_transmit(&CAN_DCU_HIL_input, portMAX_DELAY);

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(POLL_TASK_INTERVAL_MS));
    }
}