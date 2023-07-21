#ifndef CAN_LOGS_WRITER_H
#define CAN_LOGS_WRITER_HPP
#include <mutex>
#include <queue>
#include <linux/can.h>
#include <linux/can/raw.h>

void canLogsWriterRun(std::mutex* canLogsWriteQueueLock, std::queue<struct can_frame>* canLogsWriteQueue);

#endif /* CAN_LOGS_WRITER_H */