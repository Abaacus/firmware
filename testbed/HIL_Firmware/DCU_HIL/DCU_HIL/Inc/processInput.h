#ifndef PROCESS_INPUT_H_
#define PROCESS_INPUT_H_

// CAN message identifier
#define DCU_HIL_INPUT_ID 0x8010F07

/* 
 * Important: any lower than 10 ms, the serial communication will 
 * stop (the chip probably hardfaulted?)
 */

#define POLL_TASK_INTERVAL_MS 10

void poll_DCU_output_task(void *pvParameters);

#endif /* PROCESS_INPUT_H_ */