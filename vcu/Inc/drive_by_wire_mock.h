#ifndef __DRIVE_BY_WIRE_MOCK_H
#define __DRIVE_BY_WIRE_MOCK_H
#include "stm32f7xx_hal.h"
#include "stdbool.h"
#include "debug.h"

#define MIN_BRAKE_PRESSURE 5 // TODO: Set this to a reasonable value

HAL_StatusTypeDef stateMachineMockInit();
BaseType_t setBrakePosition(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t pduMCsOnOffMock(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setFakeThrottle(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setFakeThrottleAB(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getThrottle(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getSteering(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getBrake(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getFakeThrottleAB(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t getADCInputs(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setFakeBrakePressure(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t setFakeDCUCanTimeout(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t fakeEM_ToggleDCU(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t fakeHVStateChange(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t printState(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t beagleBonePower(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t torqueDemandMaxCommand(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t speedLimitCommand(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
BaseType_t mcInitCommand(char *writeBuffer, size_t writeBufferLength,
                       const char *commandString);
HAL_StatusTypeDef stateMachineMockInit();
#endif // __DRIVE_BY_WIRE_MOCK_H
