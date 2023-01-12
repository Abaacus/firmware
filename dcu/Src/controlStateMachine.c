#include "controlStateMachine.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdbool.h"

#include "bsp.h"
#include "debug.h"
#include "dcu_can.h"
#include "userCan.h"
#include "watchdog.h"
#include "canReceive.h"

#define MAIN_TASK_ID 1
#define MAIN_TASK_PERIOD_MS 1000

#define BUZZER_LENGTH_MS 2000
TimerHandle_t buzzerSoundTimer;

void buzzerTimerCallback(TimerHandle_t timer);
bool buzzerTimerStarted = false;

#define DEBOUNCE_WAIT_MS 50

TimerHandle_t debounceTimer;

void debounceTimerCallback(TimerHandle_t timer);
bool debounceTimerStarted = false;

bool TC_on = false;
bool endurance_on = false;

uint32_t selfTests(uint32_t event);
uint32_t toggleHV(uint32_t event);
uint32_t toggleEM(uint32_t event);
uint32_t toggleTC(uint32_t event);
uint32_t toggleEnduranceMode(uint32_t event);
uint32_t toggleEnduranceLap(uint32_t event);
uint32_t toggleSelect(uint32_t event);
uint32_t hvControl(uint32_t event);
uint32_t emControl(uint32_t event);
uint32_t defaultTransition(uint32_t event);
void mainTaskFunction(void const * argument);

/**
 * Update state machine from CAN message from BMU or VCU to accurately
 * reflect state on the dash.
 */
uint32_t updateFromCAN(uint32_t event)
{

    uint32_t current_state = fsmGetState(&DCUFsmHandle);
    uint32_t updated_state = current_state;

    bool hvState = getHVState();
    bool emState = getEMState();

    switch (event)
    {
        case EV_CAN_Recieve_HV:
            if (hvState == HV_Power_State_On)
            {
                if (current_state != STATE_HV_Enable)
                {
                    updated_state = STATE_HV_Enable;
                }

                DEBUG_PRINT("Rcvd HV on state, alrdy HV_enabl\n");
            }
            else /* hvState == HV_Power_State_Off */
            {
                if (current_state != STATE_HV_Disable)
                {
                    updated_state = STATE_HV_Disable;
                }

                DEBUG_PRINT("Rcvd HV off state, alrdy HV_Dsabl\n");
            }

            break;

        case EV_CAN_Recieve_EM:
            if (emState == EM_State_On)
            {
                if (current_state != STATE_EM_Enable)
                {
                    updated_state = STATE_EM_Enable;
                }

                DEBUG_PRINT("Rcvd EM on state, alrdy EM_enabl\n");
            }

            break;

        default:
            ERROR_PRINT("Unxpctd evnt whn updtng frm CAN\n");
            Error_Handler();
            break;
    }

    /* Return current state, as it's not always an error */
    /* For example, recieving multiple HV ON messages */
    return updated_state;
}

uint32_t fatalTransition(uint32_t event)
{
    return STATE_Failure_Fatal;
}

uint32_t doNothing(uint32_t event)
{
    return fsmGetState(&DCUFsmHandle);
}

Transition_t transitions[] = {
    {STATE_Self_Test,     EV_Init,                &selfTests},
    {STATE_HV_Disable,    EV_HV_Toggle,           &toggleHV},
    {STATE_HV_Toggle,     EV_CAN_Recieve_HV,      &hvControl},
    {STATE_HV_Toggle,     EV_HV_Toggle,           &doNothing},
    {STATE_HV_Toggle,     EV_CAN_Recieve_EM,      &doNothing},
    {STATE_HV_Enable,     EV_EM_Toggle,           &toggleEM},

    {STATE_EM_Toggle,     EV_CAN_Recieve_EM,      &emControl},
    {STATE_EM_Enable,     EV_EM_Toggle,           &toggleEM},
    {STATE_EM_Toggle,     EV_EM_Toggle,           &doNothing},

    {STATE_HV_Enable,     EV_HV_Toggle,           &toggleHV},

    {STATE_HV_Enable,     EV_CAN_Recieve_HV,      &updateFromCAN},
    {STATE_HV_Disable,    EV_CAN_Recieve_HV,      &updateFromCAN},

    {STATE_HV_Enable,     EV_CAN_Recieve_EM,      &updateFromCAN},
    {STATE_EM_Enable,     EV_CAN_Recieve_EM,      &updateFromCAN},
    
    // Can occur when contactors open while EM enabled
    {STATE_EM_Enable,     EV_CAN_Recieve_HV,      &updateFromCAN},

	{STATE_EM_Enable,	  EV_HV_Toggle,			  &toggleHV},
	{STATE_ANY,	          EV_TC_Toggle,			  &toggleTC},
	{STATE_EM_Enable,	  EV_Endurance_Mode_Toggle,&toggleEnduranceMode},
	{STATE_EM_Enable,	  EV_Endurance_Lap_Toggle,&toggleEnduranceLap},

    {STATE_Failure_Fatal, EV_ANY,                 &doNothing},
    {STATE_ANY,           EV_CAN_Recieve_Fatal,   &fatalTransition},
    {STATE_ANY,           EV_ANY,                 &defaultTransition}
};

uint32_t selfTests(uint32_t event)
{
    /* Run self tests here */
    DEBUG_PRINT("Cmplted strt up tsts\n");
    return STATE_HV_Disable;
}

int sendHVToggleMsg(void)
{
    ButtonHVEnabled = 1;
    ButtonEMEnabled = 0;
    ButtonEnduranceToggleEnabled = 0;
	ButtonEnduranceLapEnabled = 0;
    ButtonTCEnabled = 0;
    return sendCAN_DCU_buttonEvents();
}

int sendEMToggleMsg(void)
{
    ButtonHVEnabled = 0;
    ButtonEMEnabled = 1;
    ButtonEnduranceToggleEnabled = 0;
	ButtonEnduranceLapEnabled = 0;
    ButtonTCEnabled = 0;
    return sendCAN_DCU_buttonEvents();
}

int sendEnduranceToggleMsg(void)
{
    ButtonHVEnabled = 0;
    ButtonEMEnabled = 0;
    ButtonEnduranceToggleEnabled = 1;
	ButtonEnduranceLapEnabled = 0;
    ButtonTCEnabled = 0;
	return sendCAN_DCU_buttonEvents();
}

int sendEnduranceLapMsg(void)
{
    ButtonHVEnabled = 0;
    ButtonEMEnabled = 0;
    ButtonEnduranceToggleEnabled = 0;
	ButtonEnduranceLapEnabled = 1;
    ButtonTCEnabled = 0;
	return sendCAN_DCU_buttonEvents();
}

int sendTCToggleMsg(void)
{
    ButtonHVEnabled = 0;
    ButtonEMEnabled = 0;
    ButtonEnduranceToggleEnabled = 0;
	ButtonEnduranceLapEnabled = 0;
    ButtonTCEnabled = 1;
	return sendCAN_DCU_buttonEvents();
}

uint32_t toggleTC(uint32_t event)
{
    uint32_t currentState = fsmGetState(&DCUFsmHandle);
    if (currentState != STATE_EM_Enable)
    {
        //Can't enable TC unless at EM
        return currentState;
    }


	if(sendTCToggleMsg() != HAL_OK)
	{
        ERROR_PRINT("Fail to snd TC Tgl bttn evnt!\n");
        Error_Handler();
	}
	TC_on = !TC_on;
    if (TC_on)
    {
        TC_LED_ON;
	    DEBUG_PRINT("TC on\n");
    }
    else 
    {
        TC_LED_OFF;
	    DEBUG_PRINT("TC off\n");
    }
	return STATE_EM_Enable;
}

uint32_t toggleEnduranceMode(uint32_t event)
{
	DEBUG_PRINT("Tggling EndrncMde\n");
	if(sendEnduranceToggleMsg() != HAL_OK)
	{
        ERROR_PRINT("Fail to snd EndrMde Tgl bttn evnt!\n");
        Error_Handler();
	}
	endurance_on = !endurance_on;
	return STATE_EM_Enable;
}

uint32_t toggleEnduranceLap(uint32_t event)
{
	DEBUG_PRINT("Incrmnting Endur Lap\n");
	if(sendEnduranceLapMsg() != HAL_OK)
	{
        ERROR_PRINT("Fail to snd EndurLp Tgl bttn evnt!\n");
        Error_Handler();
	}
	return STATE_EM_Enable;
}


uint32_t toggleHV(uint32_t event)
{
    if (fsmGetState(&DCUFsmHandle) == STATE_EM_Enable)
    {
        TC_LED_OFF;
        TC_on = false;
    }

    DEBUG_PRINT("Snding HV Tgl bttn evnt\n");
    if (sendHVToggleMsg() != HAL_OK)
    {
        ERROR_PRINT("Fail to snd HV Tgl bttn evnt!\n");
        Error_Handler();
    }

    return STATE_HV_Toggle;
}

uint32_t toggleEM(uint32_t event)
{
    DEBUG_PRINT("Snding EM Tggl bttn evnt\n");
    if (sendEMToggleMsg() != HAL_OK)
    {
        ERROR_PRINT("Fail to snd EM Tgl bttn evnt!\n");
        Error_Handler();
    }

    return STATE_EM_Toggle;
}

uint32_t hvControl(uint32_t event)
{
    if(getHVState() == HV_Power_State_On)
    {
        DEBUG_PRINT("Rspnse frm BMU:HV Enbld\n");
        return STATE_HV_Enable;
    }
    else
    {
        DEBUG_PRINT("Rspnse from BMU:HV Dsbld\n");
        return STATE_HV_Disable;
    }
}

uint32_t emControl(uint32_t event)
{
    if(getEMState() == EM_State_On)
    {
        DEBUG_PRINT("Rspnse from VCU:EM Enbld\n");

        /* Only ring buzzer when going to motors enabled */
        DEBUG_PRINT("Kicking off bzzr\n");
        if (!buzzerTimerStarted)
        {
            if (xTimerStart(buzzerSoundTimer, 100) != pdPASS)
            {
                ERROR_PRINT("Fail to strt bzzr timer\n");
                Error_Handler();
            }

            buzzerTimerStarted = true;
            BUZZER_ON
        }

        return STATE_EM_Enable;
    }
    else
    {
        DEBUG_PRINT("Rspnse from VCU:EM Dsbld\n");

        //Turn off TC button
        TC_LED_OFF;
        TC_on = false;
        return STATE_HV_Enable;
    }
}

bool alreadyDebouncing = false;
uint16_t debouncingPin = 0;

/*
 * A button press is considered valid if it is still low (active) after
 * TIMER_WAIT_MS milliseconds.
 */
void debounceTimerCallback(TimerHandle_t timer)
{
    GPIO_PinState pin_val;

    switch (debouncingPin)
    {
        case HV_TOGGLE_BUTTON_PIN:
            pin_val = HAL_GPIO_ReadPin(HV_TOGGLE_BUTTON_PORT,
                    HV_TOGGLE_BUTTON_PIN);
            break;

        case EM_TOGGLE_BUTTON_PIN:
            pin_val = HAL_GPIO_ReadPin(EM_TOGGLE_BUTTON_PORT,
                    EM_TOGGLE_BUTTON_PIN);
            break;
        
        case TC_TOGGLE_BUTTON_PIN:
            pin_val = HAL_GPIO_ReadPin(TC_TOGGLE_BUTTON_PORT,
                    TC_TOGGLE_BUTTON_PIN);
            break;
         
        case ENDURANCE_TOGGLE_BUTTON_PIN:
            pin_val = HAL_GPIO_ReadPin(ENDURANCE_TOGGLE_BUTTON_PORT,
                    ENDURANCE_TOGGLE_BUTTON_PIN);
            break;

        case ENDURANCE_LAP_BUTTON_PIN:
            pin_val = HAL_GPIO_ReadPin(ENDURANCE_LAP_BUTTON_PORT,
                    ENDURANCE_LAP_BUTTON_PIN);
            break;

        default:
            /* Shouldn't get here */ 
            DEBUG_PRINT_ISR("Unkwn pin spcfied to debnce\n");
            pin_val = GPIO_PIN_SET;
            break;
    }


    if (pin_val == GPIO_PIN_RESET)
    {
        switch (debouncingPin)
        {
            case HV_TOGGLE_BUTTON_PIN:
                fsmSendEventISR(&DCUFsmHandle, EV_HV_Toggle);
                break;

            case EM_TOGGLE_BUTTON_PIN:
                fsmSendEventISR(&DCUFsmHandle, EV_EM_Toggle);
                break;
            
            case TC_TOGGLE_BUTTON_PIN:
                fsmSendEventISR(&DCUFsmHandle, EV_TC_Toggle);
                break;

			case ENDURANCE_TOGGLE_BUTTON_PIN:
                fsmSendEventISR(&DCUFsmHandle, EV_Endurance_Lap_Toggle);
				break;

			case ENDURANCE_LAP_BUTTON_PIN:
                fsmSendEventISR(&DCUFsmHandle, EV_Endurance_Mode_Toggle);
				break;

            default:
                /* Shouldn't get here */
                DEBUG_PRINT_ISR("Unkwn pin spcfied to debnce\n");
                break;
        }

    }

    alreadyDebouncing = false;
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (alreadyDebouncing)
    {
        /* Already debouncing, do nothing with this interrupt */
        return;
    }
    alreadyDebouncing = true;

    switch (pin)
    {
        case HV_TOGGLE_BUTTON_PIN:
            debouncingPin = HV_TOGGLE_BUTTON_PIN;
            break;

        case EM_TOGGLE_BUTTON_PIN:
            debouncingPin = EM_TOGGLE_BUTTON_PIN;
            break;
		
		case TC_TOGGLE_BUTTON_PIN:
			debouncingPin = TC_TOGGLE_BUTTON_PIN;
			break;

		case ENDURANCE_TOGGLE_BUTTON_PIN:
			debouncingPin = ENDURANCE_TOGGLE_BUTTON_PIN;
			break;

		case ENDURANCE_LAP_BUTTON_PIN:
			debouncingPin = ENDURANCE_LAP_BUTTON_PIN;
			break;

        default:
            /* Not a fatal error here, but report error and return */
            DEBUG_PRINT_ISR("Unknwn GPIO intrrptd in ISR\n");
            return;
            break;
    }

    xTimerStartFromISR(debounceTimer, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

HAL_StatusTypeDef dcuFsmInit(){
    FSM_Init_Struct init;
    init.maxStateNum = STATE_ANY;
    init.maxEventNum = EV_ANY;
    init.sizeofEventEnumType = sizeof(DCU_Events_t);
    init.ST_ANY = STATE_ANY;
    init.EV_ANY = EV_ANY;
    init.transitions = transitions;
    init.transitionTableLength = TRANS_COUNT(transitions);
    init.eventQueueLength = 10;
    init.watchdogTaskId = MAIN_TASK_ID;
    if (fsmInit(STATE_Self_Test, &init, &DCUFsmHandle) != HAL_OK) 
    {
        ERROR_PRINT("Fail to init DCU fsm\n");
        return HAL_ERROR;
    }

    DEBUG_PRINT("Init DCU fsm\n");
    return HAL_OK;

}

void buzzerTimerCallback(TimerHandle_t timer)
{
    buzzerTimerStarted = false;
    BUZZER_OFF
}

void mainTaskFunction(void const * argument){
    DEBUG_PRINT("Starting up!!\n");
    if (canStart(&CAN_HANDLE) != HAL_OK)
    {
        ERROR_PRINT("Fail to start CAN\n");
        Error_Handler();
    }

    buzzerSoundTimer = xTimerCreate("BuzzerTimer",
                                    pdMS_TO_TICKS(BUZZER_LENGTH_MS),
                                    pdFALSE /* Auto Reload */,
                                    0,
                                    buzzerTimerCallback);

    if (buzzerSoundTimer == NULL)
    {
        ERROR_PRINT("Fail to create bzzr timer\n");
        Error_Handler();
    }

    debounceTimer = xTimerCreate("DebounceTimer",
                                 pdMS_TO_TICKS(DEBOUNCE_WAIT_MS),
                                 pdFALSE /* Auto Reload */,
                                 0,
                                 debounceTimerCallback);

    if (debounceTimer == NULL) 
    {
        ERROR_PRINT("Fail to create dbounce timer\n");
        Error_Handler();
    }


    if (registerTaskToWatch(MAIN_TASK_ID, 5*pdMS_TO_TICKS(MAIN_TASK_PERIOD_MS), true, &DCUFsmHandle) != HAL_OK)
    {
        ERROR_PRINT("Fail to rgstr main tsk with wtchdg\n");
        Error_Handler();
    }

    fsmTaskFunction(&DCUFsmHandle);

    for(;;) {
    };
}

uint32_t defaultTransition(uint32_t event)
{
    ERROR_PRINT("No transition fncn rgstr for ");
    uint32_t currentState = fsmGetState(&DCUFsmHandle);

    //Need to do it like this since we can't do string interpolation which causes stack overflows
    if (currentState == STATE_Self_Test)
    {
        ERROR_PRINT("Self_Test state\r\n");
    }
    else if (currentState == STATE_HV_Disable)
    {
        ERROR_PRINT("HV_Disable state\r\n");
    }
    else if (currentState == STATE_HV_Toggle)
    {
        ERROR_PRINT("HV_Toggle state\r\n");
    }
    else if (currentState == STATE_HV_Enable)
    {
        ERROR_PRINT("HV_Enable state\r\n");
    }
    else if (currentState == STATE_EM_Toggle)
    {
        ERROR_PRINT("EM_Toggle state\r\n");
    }
    else if (currentState == STATE_EM_Enable)
    {
        ERROR_PRINT("EM_Enable state\r\n");
    }
    else if (currentState == STATE_Failure_Fatal)
    {
        ERROR_PRINT("Failure_Fatal state\r\n");
    }
    return currentState;
}

uint8_t getTC(void)
{
    return TC_on;
}

