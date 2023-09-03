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

/* Poll the output of the DCU board @ 10 ms */
void poll_DCU_output_task(void *pvParameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t DCU_GPIO_data = 0; 

    while (1) {
		DCU_GPIO_data = 0;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[0]) << AMS_GR_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[1]) << AMS_RED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[2]) << EM_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[3]) << HV_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[4]) << MOT_GR_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[5]) << MOT_RED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[6]) << TC_LED_BIT;
		DCU_GPIO_data |= gpio_get_level(INPUT_PIN_ARRAY[7]) << TV_LED_BIT;

		CAN_DCU_HIL_input.data[0] = DCU_GPIO_data;
		twai_transmit(&CAN_DCU_HIL_input, portMAX_DELAY); // send CAN message

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(POLL_TASK_INTERVAL_MS));
    }
}