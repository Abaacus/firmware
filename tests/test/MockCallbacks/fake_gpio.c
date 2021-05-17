#include "fake_gpio.h"
#include "unity.h"


#define NUM_GPIO_PINS 20
GPIO_PinState gpio_pins[NUM_GPIO_PINS]; 


void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  TEST_ASSERT_TRUE(IS_GPIO_PIN(GPIO_Pin));
  TEST_ASSERT_TRUE(IS_GPIO_PIN_ACTION(PinState));

  if (PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = (uint32_t)GPIO_Pin;
  }
}


void init_gpio() {
	for(int pin = 0;pin < NUM_GPIO_PINS;pin++){
		gpio_pins[pin] = GPIO_PIN_SET;
	}
}
