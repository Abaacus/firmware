#ifndef DASHBOARD_UI_H
#define DASHBOARD_UI_H

#include <vector>
#include <memory>
#include <QtWidgets>
#include <QApplication>
#include <mutex>
#include "displays/textdisplay.cpp"
#include "displays/errordisplay.cpp"
#include "displays/dial.cpp"
#include "DashData.hpp"

#define CURRENT_ROLLING_AVERAGE_PERIOD_MS 30000

class DashboardUI : public QWidget {
    private:
        // If the corresponding properties are greater than these values,
        // their respective dials display in red
        const int OVERSPEED = 200;
        const int OVERTEMP = 40;
        const int OVERVOLTAGE = 30;

        const int WIDTH = 800;
        const int HEIGHT = 480;

        // Radii of big and small dials
        const int RSMALL = 200;
        const int RBIG = 320;

        // Display instances
        std::shared_ptr<TextDisplay> currentDisplay;
        std::shared_ptr<TextDisplay> batteryDisplay;
        std::shared_ptr<TextDisplay> speedDisplay;
        std::shared_ptr<TextDisplay> lvBattDisplay;
        std::shared_ptr<TextDisplay> voltageDisplay;
        std::shared_ptr<ErrorDisplay> errorDisplay;

        // Dial instances
        std::shared_ptr<Dial> speedDial;
        std::shared_ptr<Dial> tempDial;
        std::shared_ptr<Dial> voltageDial;

        std::mutex* dashDataLock;
        DashData* lastReceivedData;
        DashData dataDisplayedOnDash;

        void updateErrorDisplay();
        std::string toXDecimalString(float floatVal, int precisionVal);
        float update30sCurrentBusHVRollingAverage(float newMeasurement);

    public:
        DashboardUI(QApplication& app, std::mutex* dashDataLock, DashData* data);
        ~DashboardUI();

        void paintEvent(QPaintEvent *);
        void update();
};

#endif /* DASHBOARD_UI_H */
