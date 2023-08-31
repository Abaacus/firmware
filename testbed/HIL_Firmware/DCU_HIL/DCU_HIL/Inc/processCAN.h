#ifndef PROCESS_CAN_H_
#define PROCESS_CAN_H_

// CAN message identifiers
#define DCU_HIL_OUTPUT_ID 0x400070F    // contains all the DCU buttons
#define DCU_HIL_OUTPUT_STATUS_ID 0x8020F07

#define PROCESS_RX_TASK_INTERVAL_MS 10

#define PIN_SET 1

void process_rx_task(void * pvParameters);

#endif /* PROCESS_CAN_H_ */