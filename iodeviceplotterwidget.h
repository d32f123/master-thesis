#ifndef IODEVICEPLOTTERWIDGET_H
#define IODEVICEPLOTTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>

#include <QtMultimedia/QAudioInput>

#include "iodeviceplotter.h"

class IODevicePlotterWidget : public QWidget {
Q_OBJECT
public:
    explicit IODevicePlotterWidget(QWidget *parent = nullptr);

    ~IODevicePlotterWidget();

    void initialize(const QAudioDeviceInfo &deviceInfo);

    void setAudio(const QByteArray &arr);

    void start() const;

    void stop() const;

    static const int sampleRate = 48000;

signals:

private:
    IODevicePlotter *plotter;

    QVBoxLayout *mainLayout;
    QtCharts::QChartView *chartView;
    QtCharts::QChart *chart;
    QtCharts::QLineSeries *series;

    QAudioInput *audioInput;
};

#endif // IODEVICEPLOTTERWIDGET_H
