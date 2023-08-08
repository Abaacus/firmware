#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/twai.h"
#include "driver/i2c.h"
#include "userInit.h"
#include "dac.h"
#include "canReceive.h"
#include "processCAN.h"

void process_rx_task (void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, 10000); // wait for adc start up ?
    while(1)
    {
        uint8_t write_buf[1] = {0b10011000}; //config byte
        int ret = i2c_master_write_to_device(0, 0b1101000, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);

        uint8_t read_buf[2];
        i2c_master_read_from_device(0, 0b1101000, read_buf, sizeof(read_buf), 1000 / portTICK_PERIOD_MS);
        printf("read data bytes: %d, %d\n", read_buf[0], read_buf[1]);

        vTaskDelayUntil(&xLastWakeTime, 10000);
    }
}