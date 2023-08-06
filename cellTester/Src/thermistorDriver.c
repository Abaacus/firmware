/**
  *****************************************************************************
  * @file    thermistorDriver.c
  * @author  Jacky Lim
  * @brief   the thermistors are connected to ADCs and the microprocessor is 
  *          communicating to the ADCs using I2C
  *****************************************************************************
  */

#include "thermistorDriver.h"

#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "math.h"

#include "debug.h"

I2C_HandleTypeDef hi2c1;    // clean up
I2C_HandleTypeDef hi2c2;    // clean up

int readThermistor(void) {
    MCP3425_Configure(CONFIG, &hi2c1);
    MCP3425_Configure(CONFIG, &hi2c2);
    while (1) {
        int temp = 0;
        uint32_t buffer = 0;
        uint32_t voltage = 0;
        uint32_t resistance = 0;

        buffer = MCP3425_ReadData(&hi2c1);
        voltage = ADC_TO_VOLTAGE(buffer);
        resistance = V_TO_R(voltage);
        temp = calculateTemp(resistance);

        CONSOLE_PRINT("Cell temp: %i\r\n", temp); // print to UART
        HAL_Delay(1000);
    }
    return 0; // return error 
}

// Send configuration command to MCP3425
void MCP3425_Configure(uint8_t config, I2C_HandleTypeDef *hi2c) {
    // Wait until I2C is ready
    while (HAL_I2C_GetState(&hi2c) != HAL_I2C_STATE_READY) {}

    // Send configuration data to MCP3425
    if (HAL_I2C_Master_Transmit(&hi2c, MCP3425_ADDRESS, &config, 1, HAL_MAX_DELAY) != HAL_OK) {
        DEBUG_PRINT("Error transmitting to peripheral\n");
    }
}

// Read ADC data from MCP3425
uint32_t MCP3425_ReadData(I2C_HandleTypeDef *hi2c) {
    uint8_t data[2]; // 2 bytes for read data

    // Wait until I2C is ready
    while (HAL_I2C_GetState(&hi2c) != HAL_I2C_STATE_READY) {}

    // Receive ADC data
    if (HAL_I2C_Master_Receive(&hi2c, MCP3425_ADDRESS, data, 2, HAL_MAX_DELAY) != HAL_OK) {
        DEBUG_PRINT("Error reading from peripheral\n");
    }

    // Process the received data to get the ADC result
    uint32_t adc_data = 0; // TODO: some complicated math

    return adc_data;
}

// calculate temperature using steinhart-hart equation
uint32_t calculateTemp(uint32_t resistance) {
    uint32_t temp = 0;
    temp = 1/(A + B*log(resistance) + C*(pow(log(resistance),3)));
    temp -= 273.15; //convert to celcius
    return temp;
}