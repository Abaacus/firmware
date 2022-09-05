#ifndef __DRIVE_BY_WIRE_MOCK_H
#define __DRIVE_BY_WIRE_MOCK_H
#include "stm32f7xx_hal.h"
#include "stdbool.h"
#include "debug.h"

BaseType_t getChannelCurrents(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getChannelsRaw(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setChannelCurrent(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setBusVoltage(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mockCritical(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mockLVCuttoff(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t channelEnableCommand(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mockEMEnableDisable(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mockHVEnableDisable(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mockOvertempWarning(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t printStates(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t testOutput(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t printPowerStates(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t controlFans(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t controlPumps(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
HAL_StatusTypeDef mockStateMachineInit();
#endif // __DRIVE_BY_WIRE_MOCK_H
