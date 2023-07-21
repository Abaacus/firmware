#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include <QApplication>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "DashData.hpp"
#include "canBusReader.hpp"
#include "canLogsWriter.hpp"
#include "DashboardUI.hpp"

static void startDashboard(int argc, char **argv, std::mutex* dashDataLock, DashData* dashData);

int main(int argc, char **argv)
{
    // Run this to make the Makefile:
    // qmake -makefile -o Makefile dashboard.pro

    // General DTC Data
    const std::string dtcFileName = "/home/debian/firmware/common/Data/DTC.csv";

    // Can Logs Data
    std::mutex canLogsWriteQueueLock;
    std::mutex dashDataLock;
    std::queue<struct can_frame> canLogsWriteQueue;

    // Dash Data
    DashData dashData;

    // Can Bus Reader Thread (Takes messages off the bus and send all messages to can logs writer and only the appropriate ones to the dash)
    std::thread canBusReader(canBusReaderRun, &canLogsWriteQueueLock, &canLogsWriteQueue, dtcFileName, &dashDataLock, &dashData);

    // Can Logs Thread (Writes to output file)
	std::thread canLogsWriter(canLogsWriterRun, &canLogsWriteQueueLock, &canLogsWriteQueue);

    // Dashbaord Thread (Draws UI shown on the screen)
    startDashboard(argc, argv, &dashDataLock, &dashData);
    return 0;
}

static void startDashboard(int argc, char **argv, std::mutex* dashDataLock, DashData* dashData)
{
    QApplication app(argc, argv);
    DashboardUI dashboard(app, dashDataLock, dashData);

    app.exec();
}
