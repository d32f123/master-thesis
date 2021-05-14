#ifndef IODEVICEPLOTTER_H
#define IODEVICEPLOTTER_H

#include <QIODevice>
#include <QObject>
#include <QVector>
#include <QPointF>

#include <QXYSeries>

class IODevicePlotter : public QIODevice {
Q_OBJECT
public:
    explicit IODevicePlotter(QtCharts::QXYSeries *series, QObject *parent = nullptr);

    void clear();


    static const int sampleCount = 16000;
    static const int resolution = 8;
    static const int resolution_power = 3;
protected:
    qint64 readData(char *data, qint64 maxSize) override;

    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QtCharts::QXYSeries *series;
    QVector<QPointF> buf;
};

#endif // IODEVICEPLOTTER_H
