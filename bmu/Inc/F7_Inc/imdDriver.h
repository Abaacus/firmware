#ifndef IMD_H
#define IMD_H

#include "FreeRTOS.h"
#include "bsp.h"

// Type of IMDSTATUS_ENUM_TO_HZ is a float. Reason to have this offset is in case a variable isnt init the default is SNA and not a SENSE_PIN_FAULT
#define IMDSTATUS_ENUM_TO_HZ(imdstatus_enum_value) ((imdstatus_enum_value-1)*10.0f)

// Tolerance for output freq from IMD has uncertainty of +/- 5%
#define IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(imdStatus) (IMDSTATUS_ENUM_TO_HZ(imdStatus) * 0.95)
#define IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(imdStatus) (IMDSTATUS_ENUM_TO_HZ(imdStatus) * 1.05)

#define IMDSTATUS_SENSE_PIN_FAULT_PWM_FREQ_MIN (0.0f) // Min measurable duty cycle (Pin at a constant value)
#define IMDSTATUS_SENSE_PIN_FAULT_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_SENSE_PIN_FAULT)

#define IMDSTATUS_NORMAL_PWM_FREQ_MIN IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(IMDSTATUS_NORMAL)
#define IMDSTATUS_NORMAL_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_NORMAL)

#define IMDSTATUS_UNDERVOLTAGE_PWM_FREQ_MIN IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(IMDSTATUS_UNDERVOLTAGE)
#define IMDSTATUS_UNDERVOLTAGE_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_UNDERVOLTAGE)

#define IMDSTATUS_RESPONSE_TIME_DEBUG_PWM_FREQ_MIN IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(IMDSTATUS_RESPONSE_TIME_DEBUG)
#define IMDSTATUS_RESPONSE_TIME_DEBUG_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_RESPONSE_TIME_DEBUG)

#define IMDSTATUS_ERROR_PWM_FREQ_MIN IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(IMDSTATUS_ERROR)
#define IMDSTATUS_ERROR_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_ERROR)

#define IMDSTATUS_FAULT_EARTH_PWM_FREQ_MIN IMDSTATUS_STATUS_MIN_ALLOWABLE_PWM_FREQ(IMDSTATUS_FAULT_EARTH)
#define IMDSTATUS_FAULT_EARTH_PWM_FREQ_MAX IMDSTATUS_STATUS_MAX_ALLOWABLE_PWM_FREQ(IMDSTATUS_FAULT_EARTH)

typedef struct {
  float meas_freq_Hz;
  float meas_duty;
  uint8_t status;
} IMDMeasurements_S;

typedef enum {
	IMDSTATUS_SNA = 0U,						// Signal Not Available
	IMDSTATUS_SENSE_PIN_FAULT,				// IMD output 0Hz: IMD Fault with the sense pin
	IMDSTATUS_NORMAL,						// IMD output 10Hz: Operation as expected (PWM Signal is expected to be ~95%
	IMDSTATUS_UNDERVOLTAGE,					// IMD output 20Hz: The IMD isn't returnidng a measurement within 2s
	IMDSTATUS_RESPONSE_TIME_DEBUG,			// IMD output 30Hz: IMD measurements fault validity: valid if PWM duty = [5,10], invalid if PWM duty = [90,95]
	IMDSTATUS_ERROR,						// IMD output 40Hz: Something is wrong with the operation of the IMD
	IMDSTATUS_FAULT_EARTH,					// IMD output 50Hz: ISOLATION FAULT. HV POTENTIAL IS EXPOSED VIA CHASIS GROUND. LV IS NOT ISOLATED FROM HV SHUT DOWN NOW!
	IMDSTATUS_TIMEOUT,						// Sense pin comms are down
	IMDSTATUS_INVALID,						// The duty cycle is invalid for the freq
} IMDStatus_E;

HAL_StatusTypeDef begin_imd_measurement(void);
HAL_StatusTypeDef stop_imd_measurement(void);
HAL_StatusTypeDef init_imd_measurement(void);

BaseType_t is_imd_faulted(IMDStatus_E* debug_status);

void publishIMDStatus(void);
#endif
