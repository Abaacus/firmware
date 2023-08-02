#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "driver/twai.h"
#include "driver/spi_master.h"
#include "canReceive.h"
#include "userInit.h"
#include "processCAN.h"
#include "digitalPot.h"
#include "pduOutputs.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

spi_device_handle_t pot;

void taskRegister (void)
{
    BaseType_t xReturned;
    TaskHandle_t can_rx_task_handler;
    TaskHandle_t can_process_task_handler;
    TaskHandle_t relay_pdu_outputs_handler;

    xReturned = xTaskCreate(
        can_rx_task,
        "CAN_RECEIVE_TASK",
        4000,
        ( void * ) 1,
        configMAX_PRIORITIES-1,
        &can_rx_task_handler
    );

    if(xReturned != pdPASS)
    {
        printf("Failed to register can_rx_task to RTOS");
    }

    xReturned = xTaskCreate(
        process_rx_task,
        "CAN_PROCESS_TASK",
        4000,
        ( void * ) 1,
        configMAX_PRIORITIES-1,
        &can_process_task_handler
    );

    if(xReturned != pdPASS)
    {
        printf("Failed to register process_rx_task to RTOS");
    }

    xReturned = xTaskCreate(
        relayPduOutputs,
        "CAN_PROCESS_TASK",
        4000,
        ( void * ) 1,
        configMAX_PRIORITIES-1,
        &relay_pdu_outputs_handler
    );

    if(xReturned != pdPASS)
    {
        printf("Failed to register relayPduOutputs to RTOS");
    }
}

int CAN_init (void)
{
    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL, 
        .tx_io = GPIO_NUM_12, 
        .rx_io = GPIO_NUM_13,
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

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        printf("Driver installed\n");
    } 
    else 
    {
        printf("Failed to install driver\n");
        return ESP_FAIL;
    }

    if (twai_start() == ESP_OK) 
    {
        printf("Driver started\n");
        return ESP_OK;
    } 
    else 
    {
        printf("Failed to start driver\n");
        return ESP_FAIL;
    }
    
}

int spi_init(void)
{
    printf("Initializing SPI bus\n");

    esp_err_t ret;
    spi_bus_config_t buscfg={
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
    };

    spi_device_interface_config_t POTcfg={
        .clock_speed_hz=25*1000*1000,           //Clock out at 25 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=POT_CS,                   //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
    };

    ret=spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    if(ret != ESP_OK){
        printf("failed to init bus\n");
    }

    ret=spi_bus_add_device(SPI2_HOST, &POTcfg, &pot);
    ESP_ERROR_CHECK(ret);
    if(ret != ESP_OK){
        printf("failed to init device\n");
    }

    return ESP_OK;
}

void pot_init(void)
{
    gpio_set_direction(POT_NSHUTDOWN, GPIO_MODE_OUTPUT);
    gpio_set_direction(POT_NSET_MID, GPIO_MODE_OUTPUT);
    gpio_set_level(POT_NSHUTDOWN, 1);   //Active low signal
    gpio_set_level(POT_NSET_MID, 1);    //Active low signal

    setPotResistance(0);
}

void pdu_input_init(void)
{
    gpio_set_direction(POW_AUX, GPIO_MODE_INPUT);
    gpio_set_direction(POW_BMU, GPIO_MODE_INPUT);
    gpio_set_direction(POW_BRAKE_LIGHT, GPIO_MODE_INPUT);
    gpio_set_direction(POW_DCU, GPIO_MODE_INPUT);
    gpio_set_direction(POW_LEFT_FAN, GPIO_MODE_INPUT);
    gpio_set_direction(POW_LEFT_PUMP, GPIO_MODE_INPUT);
    gpio_set_direction(POW_MC_LEFT, GPIO_MODE_INPUT);
    gpio_set_direction(POW_MC_RIGHT, GPIO_MODE_INPUT);
    gpio_set_direction(POW_RIGHT_FAN, GPIO_MODE_INPUT);
    gpio_set_direction(POW_RIGHT_PUMP, GPIO_MODE_INPUT);
    gpio_set_direction(POW_VCU, GPIO_MODE_INPUT);
    gpio_set_direction(BATTERY_RAW, GPIO_MODE_INPUT);
}




#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_7_BIT // Set duty resolution to 13 bits

void setPwmDutyCycle(uint32_t dutyCycle) {
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, dutyCycle));
    // Update duty to apply the new value
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}
void setPwmFreq(uint32_t freq) {
  ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_CHANNEL, freq));
}

void initPwmPin(int pinNumber, uint32_t frequency) {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = frequency,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pinNumber,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void app_main(void)
{
    CAN_init();
    // spi_init();
    pot_init();
    pdu_input_init();
    taskRegister();
    initPwmPin(3, 500000);

    while(1)
    {
    setPwmDutyCycle(64);
    }
}