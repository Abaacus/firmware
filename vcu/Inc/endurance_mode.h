#ifndef ENDURANCE_MODE_H
#define ENDURANCE_MODE_H
#include "stm32f7xx_hal.h"
#include "stdint.h"

// Config
#define NUMBER_OF_LAPS_TO_COMPLETE_DEFAULT (44)
#define ENDURANCE_MODE_BUFFER (1.05f)

void endurance_mode_EM_callback(void);
void set_lap_limit(uint32_t laps);
void trigger_lap(void);
void toggle_endurance_mode(void);
void set_initial_soc(float initial_soc_value);
void set_num_laps(uint32_t num_laps_value);
void set_num_laps_complete(uint32_t num_laps_complete_value);
void set_in_endurance_mode(bool in_endurance_mode_bool);
void set_em_kP(float em_kP_value);
void set_em_kI(float em_kI_value);

#endif
