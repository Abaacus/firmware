#ifndef DASH_DATA_H
#define DASH_DATA_H

#include <vector>
#include <string>
// #include <utility> TODO: Do we need?

class DashData {
    public:
        DashData(void);

        float speed;
        float temperature;
        float voltageCellMin;
        float soc;
        float avgBusCurrent;
        float lvBattV;
        std::vector<std::pair<int, std::string>> dtcMessagePayload;
};

#endif /* DASH_DATA_H */
