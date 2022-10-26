#ifndef DCU_STATE_MACHINE
#define DCU_STATE_MACHINE
#include "state_machine.h"
#include "cmsis_os.h"

typedef enum DCU_States_t {
    STATE_Self_Test = 0,
    STATE_HV_Disable,
    STATE_HV_Toggle,
    STATE_HV_Enable,
    STATE_EM_Toggle,
    STATE_EM_Enable,
    STATE_Failure_Fatal,
    STATE_ANY,
} DCU_States_t;

typedef enum DCU_Events_t {
    EV_Init = 0,
    EV_HV_Toggle,
    EV_EM_Toggle,
    EV_CAN_Recieve_HV,
    EV_CAN_Recieve_EM,
    EV_TC_Toggle,
    EV_Endurance_Mode_Toggle,
    EV_Endurance_Lap_Toggle,
    EV_CAN_Recieve_Fatal,
    EV_ANY,
} DCU_Events_t;

extern bool TC_on;
extern bool endurance_on;
FSM_Handle_Struct DCUFsmHandle;
HAL_StatusTypeDef dcuFsmInit();
uint32_t updateFromCAN(uint32_t event);
uint32_t fatalTransition(uint32_t event);
uint32_t doNothing(uint32_t event);
int sendHVToggleMsg(void);
int sendEMToggleMsg(void);
void HAL_GPIO_EXTI_Callback(uint16_t pin);
extern TimerHandle_t buzzerSoundTimer;
extern bool buzzerTimerStarted;
extern bool debounceTimerStarted;
extern Transition_t transitions[];
extern bool alreadyDebouncing;
extern uint16_t debouncingPin;
extern TimerHandle_t debounceTimer;

#endif
