/**
  *****************************************************************************
  * @file    imdDriver.c
  * @author  Richard Matthews
  * @brief   Module containing functions to read IMD status
  * @details The IMD (Bender IR155-3204) is an insulation monitoring
  * device which monitors the resistance between the HV Bus and the chassis
  * ground. It reports a fault if the insulation drop below a certain value.
  * The IMD communicates with the BMU through a digout gpio pin indicating a boolean
  * fault status (OK or FAIL) and a PWM signal indicating the fault type.
  *
  ******************************************************************************
  */

#include "imdDriver.h"
#include "bsp.h"
#include "debug.h"
#include "bmu_dtc.h"
#include "bmu_can.h"
#include "state_machine.h"
#include "controlStateMachine.h"

#define IMD_SENSE_PIN_FAULT    GPIO_PIN_RESET
#define IMD_SENSE_PIN_NO_FAULT GPIO_PIN_SET

// Lowest freqency expected is 10 Hz, so set timeout to twice that period
#define IMD_FREQ_MEAS_TIMEOUT_MS 200U

/* Captured Value */
volatile uint32_t            meas_IC2_val = 0;
volatile uint32_t            meas_IC1_val = 0;

/* Duty Cycle Value */
volatile float                meas_duty_cycle = 0.0f;
/* Frequency Value */
volatile float                meas_freq_Hz = 0.0f;

/* Last time we captured a pwm pulse */
volatile uint32_t lastCaptureTimeTicks = 0;

static void print_error(char err[]) {
  ERROR_PRINT("imd error: %s\n", err);
}

#define IMD_TIMER_TICK_FREQUENCY_HZ 50000
HAL_StatusTypeDef init_imd_measurement(void) {
  // Compute the right prescaler to set timer frequency
  RCC_ClkInitTypeDef      clkconfig;
  uint32_t                uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t                uwPrescalerValue = 0U;
  uint32_t                timerFrequency;
  uint32_t                pFLatency;

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;

  /* Compute timer clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }

  timerFrequency = IMD_TIMER_TICK_FREQUENCY_HZ;

  /* Compute the prescaler value to have TIM5 counter clock equal to desired
   * freqeuncy*/
  uwPrescalerValue = (uint32_t) ((uwTimclock / timerFrequency) - 1U);

  __HAL_TIM_SET_PRESCALER(&IMD_TIM_HANDLE, uwPrescalerValue);

  /* Zero out values */
  meas_IC2_val = 0;
  meas_IC1_val = 0;

  meas_duty_cycle = 0.0f;
  meas_freq_Hz = 0.0f;

  lastCaptureTimeTicks = 0;

  return HAL_OK;
}

HAL_StatusTypeDef begin_imd_measurement(void) {
  /*##-4- Start the Input Capture in interrupt mode ##########################*/
  if (HAL_TIM_IC_Start_IT(&IMD_TIM_HANDLE, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Starting Error */
    print_error("error starting status timer");
    return HAL_ERROR;
  }

  /*##-5- Start the Input Capture in interrupt mode ##########################*/
  if (HAL_TIM_IC_Start_IT(&IMD_TIM_HANDLE, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Starting Error */
    print_error("error starting status timer");
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef stop_imd_measurement(void) {
  if (HAL_TIM_IC_Stop_IT(&IMD_TIM_HANDLE, TIM_CHANNEL_2) != HAL_OK) {
    print_error("Failed to stop timer\n");
    return HAL_ERROR;
  }
  if (HAL_TIM_IC_Stop_IT(&IMD_TIM_HANDLE, TIM_CHANNEL_1) != HAL_OK) {
    print_error("Failed to stop timer\n");
    return HAL_ERROR;
  }

  return HAL_OK;
}

BaseType_t get_imd_measurements(IMDMeasurements_S* imdMeasurements) {
  BaseType_t ret = pdFALSE;

  if (((xTaskGetTickCount() - lastCaptureTimeTicks) < IMD_FREQ_MEAS_TIMEOUT_MS) &&
      (imdMeasurements != NULL))
  {
    imdMeasurements->meas_duty = meas_duty_cycle;
    imdMeasurements->meas_freq_Hz = meas_freq_Hz;
    imdMeasurements->status = HAL_GPIO_ReadPin(IMD_SENSE_GPIO_Port, IMD_SENSE_Pin);
    ret = pdTRUE;
  }
  else
  {
    // Timeout, no PWM signals from the IMD recently
    ERROR_PRINT("IMD Timed out\r\n");
  }
  return ret;
}

// param debug_status_out: debug signal if the caller wants more details about the IMD's state
BaseType_t is_imd_faulted(IMDStatus_E* debug_status) 
{
  IMDStatus_E imdStatus = IMDSTATUS_SNA;
  BaseType_t ret = pdTRUE;
  IMDMeasurements_S meas;
  
  if (get_imd_measurements(&meas) == pdTRUE)
  {
    if (meas.status == IMD_SENSE_PIN_FAULT)
    {
      ERROR_PRINT("~~~!!! IMD FAULT PIN LOW !!!~~~\r\n");
      /* If everything other than this pin looks fine, the device must be switched off */
    }

    if (meas.meas_freq_Hz < IMDSTATUS_SENSE_PIN_FAULT_PWM_FREQ_MAX)
    {
      if (meas.meas_duty == 100.0f)
      {
        imdStatus = IMDSTATUS_SENSE_PIN_FAULT;
        ERROR_PRINT("SENSE_PIN: M_HS (pin 5) is short circuit to the power pin (pin 2)\r\n");
      }
      else if (meas.meas_duty == 0.0f)
      {
        imdStatus = IMDSTATUS_SENSE_PIN_FAULT;
        ERROR_PRINT("IMDSTATUS_SENSE_PIN_FAULT: IMD off or M_HS (pin 5) is short circuit to ground\r\n");
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("SENSE_PIN_FAULT: Invalid duty of %f\r\n", meas.meas_duty);
      }
    }
    else if ((meas.meas_freq_Hz > IMDSTATUS_NORMAL_PWM_FREQ_MIN) && 
             (meas.meas_freq_Hz < IMDSTATUS_NORMAL_PWM_FREQ_MAX))
    {
      if ((meas.meas_duty >= 5.0f) && (meas.meas_duty <= 95.0f))
      {
        if (meas.meas_duty == 95.0f)
        {
          ERROR_PRINT("IMD FAULT: HV IS SHORTED TO CHASIS GROUND\r\n");
          ERROR_PRINT("BE CAREFUL AND TAKE THE NECCESSARY PRECAUTIONS TO RESOLVE THE SHORT\r\n");
        }
        else if (meas.meas_duty >= 50.0f)
        {
          ERROR_PRINT("IMD WARNING: <= 120kOhms resistance between HV+/HV- and chasis ground\r\n");
        }
        else
        {
          // All good
          ret = pdFALSE;
        }
        imdStatus = IMDSTATUS_NORMAL;
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("IMDSTATUS_NORMAL: Invalid duty of %f\r\n", meas.meas_duty);
      }
    } 
    else if ((meas.meas_freq_Hz > IMDSTATUS_UNDERVOLTAGE_PWM_FREQ_MIN) && 
             (meas.meas_freq_Hz < IMDSTATUS_UNDERVOLTAGE_PWM_FREQ_MAX))
    {
      if ((meas.meas_duty >= 5.0f) && (meas.meas_duty <= 95.0f)){
        // The potential difference between HV+ and HV- falls below a supplier programmed undervoltage threshold that we specified at the time of ordering the part
        ERROR_PRINT("IMDSTATUS_UNDERVOLTAGE: Battery undervoltage\r\n");
        imdStatus = IMDSTATUS_UNDERVOLTAGE;
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("IMDSTATUS_UNDERVOLTAGE: Invalid duty of %f\r\n", meas.meas_duty);
      }
    } 
    else if ((meas.meas_freq_Hz > IMDSTATUS_RESPONSE_TIME_DEBUG_PWM_FREQ_MIN) && 
             (meas.meas_freq_Hz < IMDSTATUS_RESPONSE_TIME_DEBUG_PWM_FREQ_MAX))
    {
      if ((meas.meas_duty >= 5.0f) && (meas.meas_duty <= 10.0f))
      {
        // Good response time, nothing to worry about
        ret = pdFALSE;
        imdStatus = IMDSTATUS_RESPONSE_TIME_DEBUG;
      } 
      else if ((meas.meas_duty >= 90.0f) && (meas.meas_duty <= 95.0f))
      {
        //Response time was not within 2s
        imdStatus = IMDSTATUS_RESPONSE_TIME_DEBUG;
        //Response time for the first measurement of the system state (SST) >= 2s
        ERROR_PRINT("IMDSTATUS_RESPONSE_TIME_DEBUG: SST response time is bad\r\n");
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("IMDSTATUS_RESPONSE_TIME_DEBUG: Invalid duty of %f\r\n", meas.meas_duty);
      }
    }
    else if ((meas.meas_freq_Hz > IMDSTATUS_ERROR_PWM_FREQ_MIN) && 
             (meas.meas_freq_Hz < IMDSTATUS_ERROR_PWM_FREQ_MAX))
    {
      if ((meas.meas_duty >= 47.5f) && (meas.meas_duty < 53.5f))
      {
        imdStatus = IMDSTATUS_ERROR;
        // There is probably something wrong with the IMD
        ERROR_PRINT("IMDSTATUS_ERROR: IMD reported an error\r\n");
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("IMDSTATUS_ERROR: Invalid duty of %f\r\n", meas.meas_duty);
      }
    }
    else if ((meas.meas_freq_Hz > IMDSTATUS_FAULT_EARTH_PWM_FREQ_MIN) && 
             (meas.meas_freq_Hz < IMDSTATUS_FAULT_EARTH_PWM_FREQ_MAX))
    {
      if ((meas.meas_duty > 47.5f) && (meas.meas_duty < 53.5f))
      {
        imdStatus = IMDSTATUS_FAULT_EARTH;
        // HV POTENTIAL IS EXPOSED VIA CHASIS GROUND. LV IS NOT ISOLATED FROM HV SHUT DOWN NOW!
        ERROR_PRINT("IMDSTATUS_FAULT_EARTH: !!!INSULATION FAULT!!! DISCONNECT THE HV BUS\r\n");
      }
      else
      {
        imdStatus = IMDSTATUS_INVALID;
        // Shouldn't happen. Something is probably wrong with IMD or input capture
        ERROR_PRINT("IMDSTATUS_FAULT_EARTH: Invalid duty of %f\r\n", meas.meas_duty);
      }
    }
    else
    {
      ERROR_PRINT("Measured unmapped PWM freq of %f\r\n", meas.meas_freq_Hz);
    }
  }
  else
  {
    imdStatus = IMDSTATUS_TIMEOUT;
  }

  if (debug_status != NULL)
  {
    *debug_status = imdStatus;
  }
  return ret;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == IMD_TIM_INSTANCE) { // measurement signal from IMD (meas)
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      /* Get the Input Capture value */
      meas_IC2_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

      if (meas_IC2_val != 0)
      {
        /* Duty cycle computation */
        meas_IC1_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        meas_duty_cycle = (((float)meas_IC1_val) * 100) / ((float)meas_IC2_val);

        /* meas_freq_Hz computation */
        meas_freq_Hz = ((float)IMD_TIMER_TICK_FREQUENCY_HZ) / ((float)meas_IC2_val);

        lastCaptureTimeTicks = xTaskGetTickCountFromISR();
      }
      else
      {
        meas_duty_cycle = 0.0f;
        meas_freq_Hz = 0.0f;
      }
    }
  }
}

void publishIMDStatus(void)
{
  IMDFrequency = (uint8_t)meas_freq_Hz;
  IMDDutyCycle = (uint8_t)meas_duty_cycle;
  sendCAN_BMU_IMD_Status();
}
