#include "DashboardUI.hpp"
#include "DashData.hpp"
#include <iostream>

#define DASH_UPDATE_PERIOD_MS 150

DashboardUI::DashboardUI(QApplication& app, std::mutex* dashDataLock, DashData* dataReadOnBus) {
    this->lastReceivedData = dataReadOnBus;
    this->dashDataLock = dashDataLock;

    // Set window title
    this->setWindowTitle("WFE Dashboard");

    // Set window size
    this->setGeometry(10, 10, WIDTH, HEIGHT);

    // Set window position to center of screen
    QRect qtRectangle = this->frameGeometry();
    QPoint centerPoint = app.primaryScreen()->availableGeometry().center();
    qtRectangle.moveCenter(centerPoint);
    this->move(qtRectangle.topLeft());

    // Set background to black
    QPalette pal = this->palette();
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    // Initialize display instances
    currentDisplay = std::make_shared<TextDisplay>(this, "Current: 0 A", Qt::white, Qt::AlignLeft);
    batteryDisplay = std::make_shared<TextDisplay>(this, "SOC: 0%", Qt::green, Qt::AlignRight);
    speedDisplay = std::make_shared<TextDisplay>(this, "Speed: 0 kph", Qt::white, Qt::AlignHCenter);
    lvBattDisplay = std::make_shared<TextDisplay>(this, "Lipo: 0 V", Qt::white, Qt::AlignLeft, 22, 45);
    voltageDisplay = std::make_shared<TextDisplay>(this, "Cell Min: 0 V", Qt::white, Qt::AlignRight, 22, 45);
    errorDisplay = std::make_shared<ErrorDisplay>(this, Qt::AlignBottom);

    // Initialize dial instances
    speedDial = std::make_shared<Dial>(this, QColor(44, 197, 239), Qt::red, WIDTH/2 - RBIG/2, 100, RBIG,
                                       20, 0, 280, OVERSPEED, 210, -240, "kph", 0);

    tempDial = std::make_shared<Dial>(this, QColor(255, 204, 0), Qt::red, 90, 200, RSMALL, 15, -20,
                                      80, OVERTEMP, 210, -140, "°C", -20);

    voltageDial = std::make_shared<Dial>(this, QColor(181, 124, 255), Qt::red, WIDTH-90-RSMALL, 200, RSMALL,
                                         15, -20, 60, OVERVOLTAGE, -30, 140, "V", 20);
    speedDial->setValue(0);
    voltageDial->setValue(0);
    tempDial->setValue(0);

    // Update values every 10 milliseconds
    QTimer *timer = new QTimer(this);
    timer->setInterval(DASH_UPDATE_PERIOD_MS);
    connect(timer, &QTimer::timeout, this, &DashboardUI::update);
    timer->start();
    this->show();
}

DashboardUI::~DashboardUI(){}

void DashboardUI::updateErrorDisplay() {
    for(std::pair<int, std::string> i : this->dataDisplayedOnDash.dtcMessagePayload) {
        errorDisplay->addErrorMessage(i.second, i.first);
    }
}

std::string DashboardUI::toXDecimalString(float floatVal, int precisionVal) {
    return std::to_string(floatVal).substr(0, std::to_string(floatVal).find(".") + precisionVal + 1);
}

void DashboardUI::update() {
    // Read latest value 
    this->dashDataLock->lock();
    this->dataDisplayedOnDash = *(this->lastReceivedData);
    this->lastReceivedData->dtcMessagePayload.clear();
    this->dashDataLock->unlock();

    updateErrorDisplay();

    batteryDisplay->setText("SOC: " + toXDecimalString(dataDisplayedOnDash.soc, 2) + "%");
    if(dataDisplayedOnDash.soc > 66) {
        batteryDisplay->setColour(Qt::green);
    } else if(dataDisplayedOnDash.soc > 33) {
        batteryDisplay->setColour(Qt::yellow);
    } else {
        batteryDisplay->setColour(Qt::red);
    }

    speedDisplay->setText("Speed: " + toXDecimalString(dataDisplayedOnDash.speed, 2) + " kph");
    lvBattDisplay->setText("Lipo: "+ toXDecimalString(dataDisplayedOnDash.lvBattV, 2) + " V");
    voltageDisplay->setText("Cell Min: "+ toXDecimalString(dataDisplayedOnDash.voltageCellMin, 2) +" V");
    currentDisplay->setText("Current: "+ toXDecimalString(dataDisplayedOnDash.avgBusCurrent, 2) +" A");

    speedDial->setValue(dataDisplayedOnDash.speed);
    tempDial->setValue(dataDisplayedOnDash.temperature);
    voltageDial->setValue(dataDisplayedOnDash.voltageCellMin);
    this->repaint();
}

void DashboardUI::paintEvent(QPaintEvent *) {
    QPainter qp(this);
    QMatrix matrix;
    matrix.translate(WIDTH, HEIGHT);
    matrix.scale(-1, -1);
    qp.setMatrix(matrix);

    currentDisplay->draw(qp);
    batteryDisplay->draw(qp);
    speedDisplay->draw(qp);
    lvBattDisplay->draw(qp);
    voltageDisplay->draw(qp);
    errorDisplay->draw(qp);
    speedDial->draw(qp);
    tempDial->draw(qp);
    voltageDial->draw(qp);
}

