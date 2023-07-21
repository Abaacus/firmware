#include <QtWidgets>
#include <QApplication>

#define DIAL_VALUE_NUM_CHARACTERS 10

class Dial {
private:
    QWidget* parent;
    QColor normColour;
    QColor overColour;
    int x;
    int y;
    int radius;
    int thickness;
    float val;
    float minVal;
    float maxVal;
    float overVal;
    int startAng;
    int spanAng;
    std::string text;
    int textXOffset;
    bool isOver;
    int valFontSize;
    int textFontSize;

public:
    Dial(
        QWidget* parent,
        QColor normColour,
        QColor overColour,
        int x,
        int y,
        int radius,
        int thickness,
        float minVal,
        float maxVal,
        float overVal,
        int startAng,
        int spanAng,
        std::string text,
        int textXOffset
    ) {
        this->parent = parent;
        this->normColour = normColour;
        this->overColour = overColour;
        this->x = x;
        this->y = y;
        this->radius = radius;
        this->thickness = thickness;
        this->val = minVal;
        this->minVal = minVal;
        this->maxVal = maxVal;
        this->overVal = overVal;
        this->startAng = startAng * 16;
        this->spanAng = spanAng * 16;
        this->text = text;
        this->textXOffset = textXOffset;
        this->isOver = this->val >= this->overVal;
        this->valFontSize = this->radius / 8;
        this->textFontSize = this->radius / 16;
    }

    void draw(QPainter& qp) {
        // Grey arc, shows total span of dial
        qp.setPen(QPen(Qt::darkGray, thickness, Qt::SolidLine));
        qp.drawArc(x, y, radius, radius, startAng, spanAng);

        QPen pen = QPen(isOver ? overColour : normColour, thickness, Qt::SolidLine);
        qp.setPen(pen);
        int localSpanAng = round(((val - minVal*1.0) / (maxVal - minVal)) * spanAng);

        // Coloured arc on top of grey arc, shows current span of dial
        qp.drawArc(x, y, radius, radius, startAng, localSpanAng);

        qp.setPen(Qt::white);
        qp.setFont(QFont(QString("Arial"), valFontSize));

        // Round the value
        char roundedValue[DIAL_VALUE_NUM_CHARACTERS];
        memset(roundedValue, '\0', DIAL_VALUE_NUM_CHARACTERS);
        snprintf(roundedValue, DIAL_VALUE_NUM_CHARACTERS, "%.1f", val);

        // Display value
        qp.drawText(x + textXOffset,
                    (int)(y - radius * 0.075),
                    radius,
                    radius,
                    Qt::AlignCenter,
                    QString(roundedValue));
        qp.setPen(isOver ? overColour : normColour);
        qp.setFont(QFont(QString("Arial"), textFontSize));

        // Display smaller text (the unit) e.g. "kph" in speed dial
        qp.drawText(x + textXOffset,
                    (int)(y + radius * 0.075),
                    radius,
                    radius,
                    Qt::AlignCenter,
                    QString::fromStdString(text));
    }

    void setValue(float v) {
        val = v;
        isOver = val > overVal;
    }
};
