#include "canLogsWriter.hpp"
#include <stdint.h>
#include <fstream>
#include <mutex>
#include <time.h>
#include <queue>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <memory>
#include <cstring>
#include <sys/stat.h>


#define MAX_CANLOG_LINE_LEN 250
#define MAX_FILENAME_LEN 50
#define MS_PER_S 1000.f

void canLogsWriterRun(	std::mutex* canLogsWriteQueueLock, 
						std::queue<struct can_frame>* canLogsWriteQueue)
{
	char output[MAX_CANLOG_LINE_LEN];
	memset(output, '\0', MAX_CANLOG_LINE_LEN);

	struct stat st;
	memset(&st, 0, sizeof(struct stat));
	const char* canLogsDir = "/home/debian/can_logs/";

	if (stat(canLogsDir, &st) == -1) {
		mkdir(canLogsDir, 0700);
	}
	
	char fileName[MAX_FILENAME_LEN];

	uint16_t fileNum = 0U;

	FILE* fp = NULL;
	do {
		snprintf(fileName, MAX_FILENAME_LEN, "/home/debian/can_logs/log_%u.csv", fileNum);
		fileNum++;
		fp = fopen(fileName, "r");
	} while (fp);

	std::ofstream logFile; 
	logFile.open(fileName); 
	logFile << "Ms Since Boot, CAN ID, Value,\n"; 

	while(1)
	{
		struct can_frame frame;
		memset(&frame, 0, sizeof(struct can_frame));

		bool gotData = false;
		while(!gotData)
		{
			(*canLogsWriteQueueLock).lock();
			if (!(*canLogsWriteQueue).empty())
			{
				frame = (*canLogsWriteQueue).front();
				(*canLogsWriteQueue).pop();
				gotData = true;
			}
			(*canLogsWriteQueueLock).unlock();
		}

		snprintf(output, MAX_CANLOG_LINE_LEN, "%.3f, %u, %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X,\n", clock() * MS_PER_S / CLOCKS_PER_SEC, frame.can_id, frame.data[7], frame.data[6], frame.data[5], frame.data[4], frame.data[3], frame.data[2], frame.data[1], frame.data[0]);
		logFile << output;
	}
}