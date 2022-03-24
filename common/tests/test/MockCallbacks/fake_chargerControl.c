#include "fake_chargerControl.h"
#include "can_interfaces.h"
#include "debug.h"

HAL_StatusTypeDef chargerInit()
{
   if (canStart(&CHARGER_CAN_HANDLE) != HAL_OK) {
      ERROR_PRINT("Failed to start charger CAN\n");
   }

   return HAL_OK;
}