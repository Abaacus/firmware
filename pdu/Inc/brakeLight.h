#ifndef BRAKELIGHT_H

#define BRAKELIGHT_H
#include "bsp.h"

HAL_StatusTypeDef setBrakeLightOnThreshold(float brake_light_on_threshold_unit);
float getBrakeLightOnThreshold(void);

#endif /* end of include guard: BRAKELIGHT_H */
