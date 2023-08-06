/*
 * thermistorDriver.h
 *
 *  Created on: August 5, 2023
 *      Author: Jacky
 */

#ifndef THERMISTOR_DRIVER_H_
#define THERMISTOR_DRIVER_H_

#define MCP3425_ADDRESS 0x68 << 1 // 0th bit is reserved for R/W bit. Same address for both
#define CONFIG 0x98 // config: set resolution to 15 SPS (16 bits). Rest is default 
#define OUTPUT_REG 

#define ADC_RES 16  // -32768 to 32767
#define V_REF 3.3
#define ADC_TO_VOLTAGE(adc_data) ((V_REF / ((1 << ADC_BITS) - 1)) * (adc_data))

// formula for wheatstone bridge 
#define R 10000
#define V_TO_R(voltage) (R((voltage/V_REF)+(V_REF/2*V_REF)))/(1-((voltage/V_REF)+(V_REF/2*V_REF)))

/* steinhart-hart coefficients (TODO: may need to be adjusted)
 * Sample data: -30, 20, 70 celcius. May want to change it to expected range
 */ 
#define A 0.00112766
#define B 0.000234372
#define C 0.0000000866770

// Function prototypes
void MCP3425_Configure(uint8_t config, I2C_HandleTypeDef *hi2c);
uint32_t MCP3425_ReadData(I2C_HandleTypeDef *hi2c);
uint32_t calculateTemp(uint32_t resistance);

#endif /* THERMISTOR_DRIVER_H_ */