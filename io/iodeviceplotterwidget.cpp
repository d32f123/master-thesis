#include "iodeviceplotterwidget.h"

#include <QtCharts/QValueAxis>

#include "iodevicerecorder.h"

using namespace QtCharts;

IODevicePlotterWidget::IODevicePlotterWidget(QWidget *parent)
        : QWidget(parent),
          chart(new QChart), series(new QLineSeries), audioInput(nullptr) {
    mainLayout = new QVBoxLayout(this);
    chartView = new QChartView(chart);

    chartView->setMinimumSize(400, 250);
    chart->addSeries(series);

    QValueAxis *xs = new QValueAxis;
    xs->setRange(0, IODevicePlotter::sampleCount);
    xs->setLabelFormat("%g");
    xs->setTitleText("Time");

    QValueAxis *ys = new QValueAxis;
    ys->setRange(-1, 1);
    ys->setTitleText("Audio level");

    chart->addAxis(xs, Qt::AlignBottom);
    chart->addAxis(ys, Qt::AlignLeft);

    series->attachAxis(xs);
    series->attachAxis(ys);

    chart->legend()->hide();

    mainLayout->addWidget(chartView);

    plotter = new IODevicePlotter(series, this);
    plotter->open(QIODevice::WriteOnly);
}

void IODevicePlotterWidget::initialize(const QAudioDeviceInfo &deviceInfo) {
    chart->setTitle("Input from " + deviceInfo.deviceName());
    if (audioInput != nullptr) {
        delete audioInput;
    }
    audioInput = new QAudioInput(deviceInfo, IODeviceRecorder::defaultFormat(), this);
}

void IODevicePlotterWidget::setAudio(const QByteArray &arr) {
    plotter->clear();
    plotter->write(arr);
}

void IODevicePlotterWidget::start() const {
    plotter->clear();
    if (audioInput != nullptr) {
        audioInput->start(plotter);
    }
}

void IODevicePlotterWidget::stop() const {
    if (audioInput != nullptr) {
        audioInput->stop();
    }
}

IODevicePlotterWidget::~IODevicePlotterWidget() {
    if (audioInput != nullptr) {
        audioInput->stop();
    }
    plotter->close();
}
