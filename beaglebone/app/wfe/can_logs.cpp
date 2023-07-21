/*
 * To compile run "g++ can_logs.cpp -pthread -o /home/debian/run_can_logs"
 */

#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>
#include <time.h>
#include <mutex>
#include <queue>
#include <stdint.h>

std::mutex lock;
std::queue<struct can_frame> q;

#define MAX_CANLOG_LINE_LEN 250
#define MAX_FILENAME_LEN 50

void writer(void)
{
	char output[MAX_CANLOG_LINE_LEN];
	memset(output, '\0', MAX_CANLOG_LINE_LEN);

	struct stat st = {0};
	const char* canLogsDir = "/home/debian/can_logs/";

	if (stat(canLogsDir, &st) == -1) {
		mkdir(canLogsDir, 0700);
	}
	
	char fileName[MAX_FILENAME_LEN];

	uint16_t fileNum = 0U;

	FILE* fp = NULL;
	do {
		snprintf(fileName, MAX_FILENAME_LEN, "/home/debian/can_logs/log_%lu.csv", fileNum);
		fileNum++;
		fp = fopen(fileName, "r");
	} while (fp);

	std::ofstream logFile; 
	logFile.open(fileName); 
	logFile << "ms since boot, CAN ID, Value,\n"; 

	while(1)
	{
		struct can_frame frame;
		memset(&frame, 0, sizeof(struct can_frame));
		while(q.empty()){}
		lock.lock();
		frame = q.front();
		q.pop();
		lock.unlock();

		snprintf(output, MAX_CANLOG_LINE_LEN, "%lu, %lu, %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X,\n", clock(), frame.can_id, frame.data[7],frame.data[6],frame.data[5],frame.data[4],frame.data[3],frame.data[2],frame.data[1],frame.data[0]);
		logFile << output;
	}
}

void counter(void)
{
	while(1)
	{
		printf("%u\n", q.size());
		sleep(0.25);
	}
}

int main()
{
	int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	struct ifreq ifr; 
	strcpy (ifr.ifr_name, "can1");
	struct can_frame frame;
	memset(&frame, 0 , sizeof(struct can_frame));
	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));

	if (sock < 0)
	{
		std::cout << "cant create socket" << std::endl;
	}

	ioctl(sock, SIOCGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::cout << "cant bind socket" << std::endl;
	}
	std::thread writeThread(writer);

	// Enable this thread if there are memory concerns with this process
	// The script was tested with 97% bus load and it works
	//std::thread debugThread(counter);

	while (1)
	{
		int nbytes = read(sock, &frame, sizeof(struct can_frame));
		if (nbytes <= 0)
		{
			continue;
		}
		q.emplace(frame);
	}
}
