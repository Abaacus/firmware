#ifndef CAN_BUS_READER_H
#define CAN_BUS_READER_H

#include <mutex>
#include <queue>
#include <memory>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string>

#include "DashData.hpp"

int canBusReaderRun(std::mutex* canLogsWriteQueueLock, std::queue<struct can_frame>* canLogsWriteQueue, const std::string dtcFileName, std::mutex* dashDataLock, DashData* dashData);

#endif /* CAN_BUS_READER_H*/
