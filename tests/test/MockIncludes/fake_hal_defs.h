#ifndef FAKE_HAL_DEFS_H
#define FAKE_HAL_DEFS_H

#ifdef BOARD_TYPE_F7
#include "stm32f767xx.h"
#include "stm32f7xx.h"
#else
#ifdef BOARD_TYPE_F0
#include "stm32f0xx.h"
#endif
#endif

#endif
