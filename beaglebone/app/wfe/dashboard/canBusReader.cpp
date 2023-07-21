#include "canBusReader.hpp"

#include <iostream>
#include <sys/socket.h>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>
#include <mutex>
#include <queue>
#include <memory>
#include <regex>
#include <algorithm>

#include "csv.h"
#include "DashData.hpp"

#define GET_SIGNAL(signalStartBit, signalLength, factor, offset, isSigned) (extractSignalValue(frame->data, signalStartBit, signalLength, factor, offset, isSigned))

#define CURRENT_BUS_HV_PUBLISHING_FREQ_HZ 10
#define DASH_CURRENT_AVERAGE_LENGTH_SEC 30 // Use an average of the last 30s of measurements
#define CURRENT_AVERAGE_QUEUE_SIZE (DASH_CURRENT_AVERAGE_LENGTH_SEC)
#define MV_PER_V 1000.0f

typedef union
{
    uint64_t    unsignedData;
    int64_t     signedData;
} CanFrameData_U;
    
static void parseRawCanBusDataForDash(struct can_frame* frame, std::vector<std::string>& dtcList, std::mutex* dashDataLock, DashData* dashData);
static void createDtcArray(const std::string dtc_fname, std::vector<std::string>& dtcList);
static void updateBusCurrentAverage(float latestReading);
static float getBusCurrentAverage(void);
static float extractSignalValue(uint8_t* data, uint8_t signalStartBit, uint8_t signalLength, float factor, float offset, bool isSigned);

static const std::vector<int> EmEnableFailCodes = {16, 17};
static const std::vector<std::string> EmEnableFailReasons = {   "bpsState false", 
                                                                "low brake pressure", 
                                                                "throttle non-zero",
                                                                "brake not pressed", 
                                                                "not hv enabled", 
                                                                "motors failed to start" };
static std::queue<float> oneSecondCurrentAverages;
static float oneSecondCurrentAveragesAccumulation = 0.0f;

int canBusReaderRun(std::mutex* canLogsWriteQueueLock, 
                    std::queue<struct can_frame>* canLogsWriteQueue,
                    const std::string dtcFileName, 
                    std::mutex* dashDataLock, 
                    DashData* dashData)
{
    // Parse DTC file
    std::vector<std::string> dtcList;
    createDtcArray(dtcFileName, dtcList);

    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    struct ifreq ifr; 
    strcpy (ifr.ifr_name, "can1");
    struct can_frame frame;
    memset(&frame, 0 , sizeof(struct can_frame));
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));

    if (sock < 0)
    {
        return 1;
    }

    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    while (1)
    {
        int nbytes = read(sock, &frame, sizeof(struct can_frame));
        if (nbytes <= 0)
        {
            continue;
        }
        parseRawCanBusDataForDash(&frame, dtcList, dashDataLock, dashData);

        canLogsWriteQueueLock->lock();
        (*canLogsWriteQueue).push(frame);
        canLogsWriteQueueLock->unlock();
    }
    return 0;
}

static void parseRawCanBusDataForDash(	struct can_frame* frame,
                                        std::vector<std::string>& dtcList,
                                        std::mutex* dashDataLock,
                                        DashData* dashData)
{
    // BMU_stateBusHV
    if (frame->can_id == 2282754561) {
        const float voltageCellMin = GET_SIGNAL(32, 16, 0.0001, 0, false);
        const float currentBusHV = GET_SIGNAL(48, 16, 0.01, 0, false);
        updateBusCurrentAverage(currentBusHV);

        dashDataLock->lock();
        dashData->avgBusCurrent = getBusCurrentAverage();
        dashData->voltageCellMin = voltageCellMin;
        dashDataLock->unlock();
    }
    // BMU_batteryStatusHV
    else if (frame->can_id == 2281967617) {
        const float stateBatteryChargeHv = GET_SIGNAL(0, 10, 0.1, 0, false);
        const float tempCellMax = GET_SIGNAL(32, 10, 0.25, 90, true);

        dashDataLock->lock();
        dashData->soc = stateBatteryChargeHv;
        dashData->temperature = tempCellMax;
        dashDataLock->unlock();
    }
    // WheelSpeedKPH
    else if (frame->can_id == 2287471618) {
        const float flSpeedKPH = GET_SIGNAL(32, 16, 0.04, 0, true);
        const float frSpeedKPH = GET_SIGNAL(48, 16, 0.04, 0, true);
        const float avgFrontSpeedKPH = (flSpeedKPH + frSpeedKPH) / 2;

        dashDataLock->lock();
        dashData->speed = avgFrontSpeedKPH;
        dashDataLock->unlock();
    }
    // PDU_batteryStatusLV
    else if (frame->can_id == 2551190019)
    {
        const float VoltageBusLV_mV = GET_SIGNAL(0, 14, 1, 0, false);
        dashDataLock->lock();
        dashData->lvBattV = VoltageBusLV_mV / MV_PER_V;
        dashDataLock->unlock();
    }
    // Read DTC messages. All ID's start with 21475489xx
    // PDU_DTC, DCU_DTC, VCU_F7_DTC, BMU_DTC
    else if (frame->can_id >= 2147548900 && frame->can_id <= 2147548999)
    {
        const int dtc_code = (int)frame->data[0];
        const int dtc_severity = (int)frame->data[1];
        const int32_t dtc_data = (int32_t)((frame->data[5] << 24) | (frame->data[4] << 16) | (frame->data[3] << 8) | (frame->data[2]));

        std::string message;
        if (dtc_code <= 0 || (unsigned)dtc_code > dtcList.size()) {
            message = "Out of range DTC code: " + std::to_string(dtc_code) + "\nMax DTC code: " + std::to_string(dtcList.size());
        }
        else {
            message = dtcList[dtc_code-1];
        }

        if(std::find(EmEnableFailCodes.begin(), EmEnableFailCodes.end(), dtc_code) != EmEnableFailCodes.end()) {
            std::size_t endIndex = message.find(" (Reasons");
            message = message.substr(0, endIndex);
            message = std::regex_replace(message, std::regex("#data"), EmEnableFailReasons[dtc_data]);
        } else {
            message = std::regex_replace(message, std::regex("#data"), std::to_string(dtc_data));
        }
        std::pair<int, std::string> newMsg = std::make_pair(dtc_severity, message);
        dashDataLock->lock();
        dashData->dtcMessagePayload.push_back(newMsg);
        dashDataLock->unlock();
    }
}

static void createDtcArray(const std::string dtc_fname, std::vector<std::string>& dtcList)
{
    io::CSVReader<7, io::trim_chars<>, io::double_quote_escape<',', '\"'>> in(dtc_fname);
    in.read_header(io::ignore_extra_column, "DTC CODE" , "NAME", "ORIGIN", "SEVERITY", "SUBSCRIBERS", "DATA", "MESSAGE");

    int dtc_code, severity;
    std::string name, origin, subscribers, dashData, message;

    while (in.read_row(dtc_code, name, origin, severity, subscribers, dashData, message)) {
        dtcList.push_back(message);
    }
}

/* 
    Since there is a high publishing rate for currentBusHV, we don't want to store all the values we read to get an average.
    Instead, we store one second averages and using CURRENT_AVERAGE_QUEUE_SIZE to determine how much history we want to keep track of
*/
static void updateBusCurrentAverage(float latestReading)
{
    static float current1sAverageAccumulation = 0U;
    static unsigned int current1sAverageNumReadings = 0U;

    ++current1sAverageNumReadings;
    current1sAverageAccumulation += latestReading;

    if (current1sAverageNumReadings >= CURRENT_BUS_HV_PUBLISHING_FREQ_HZ)
    {
        if (oneSecondCurrentAverages.size() >= CURRENT_AVERAGE_QUEUE_SIZE)
        {
            oneSecondCurrentAveragesAccumulation -= oneSecondCurrentAverages.front();
            oneSecondCurrentAverages.pop();
        }

        const float last1sCurrentAverage = current1sAverageAccumulation / current1sAverageNumReadings;
        oneSecondCurrentAveragesAccumulation += last1sCurrentAverage;
        oneSecondCurrentAverages.push(last1sCurrentAverage);
        current1sAverageNumReadings = 0U;
        current1sAverageAccumulation = 0U;
    }
}

static float getBusCurrentAverage(void)
{
    float ret = 0.0f;
    if (oneSecondCurrentAverages.size() > 0)
    {
        ret = (oneSecondCurrentAveragesAccumulation / oneSecondCurrentAverages.size());
    }
    return ret;
}

float extractSignalValue(uint8_t* data, uint8_t signalStartBit, uint8_t signalLength, float factor, float offset, bool isSigned)
{
    float ret = 0.0f;
    CanFrameData_U frameData;
    frameData.unsignedData = (((uint64_t)data[7] << 56) | ((uint64_t)data[6] << 48) | ((uint64_t)data[5] << 40) | ((uint64_t)data[4] << 32) | (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | (data[0] << 0));
    if (signalStartBit + signalLength > 64)
    {
        return ret;
    }

    // Remove bits above signal
    const uint8_t excessMSB = (64-(signalStartBit+signalLength));
    const uint64_t mask = (0xFFFFFFFFFFFFFFFF << excessMSB) >> excessMSB;
    frameData.unsignedData &= mask;

    // Remove bits below signal
    frameData.unsignedData = frameData.unsignedData >> signalStartBit;

    if (isSigned)
    {
        if (frameData.signedData & (1 << (signalLength-1)))
        {
            // Sign extend
            frameData.signedData |= (0xFFFFFFFFFFFFFFFF << signalLength);
        }
        ret = frameData.signedData;
    }
    else 
    {
        ret = frameData.unsignedData;
    }
    return (ret * factor) + offset;
}
