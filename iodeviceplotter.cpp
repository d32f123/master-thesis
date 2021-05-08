#include "iodeviceplotter.h"

using namespace QtCharts;

IODevicePlotter::IODevicePlotter(QXYSeries *series, QObject *parent)
        : QIODevice(parent), series(series), buf() {}

void IODevicePlotter::clear() {
    series->clear();
    for (int s = 0; s < buf.size(); ++s) {
        buf[s].setY(qreal(0));
    }
}

qint64 IODevicePlotter::readData(char *, qint64) {
    return -1;
}

qint64 IODevicePlotter::writeData(const char *data, qint64 maxSize) {
    if (buf.isEmpty()) {
        buf.reserve(IODevicePlotter::sampleCount);
        for (int i = 0; i < IODevicePlotter::sampleCount; ++i) {
            buf.append(QPointF(i, 0));
        }
    }

    int start = 0;
    const int availableSamples = int(maxSize) / resolution;
    if (availableSamples < sampleCount) {
        start = sampleCount - availableSamples;
        for (int s = 0; s < start; ++s) {
            buf[s].setY(buf[s + availableSamples].y());
        }
    }

    for (int s = start; s < sampleCount; ++s, data += IODevicePlotter::resolution) {
        buf[s].setY(qreal(uchar(*data) - 128) / qreal(128));
    }

    series->replace(buf);
    return (sampleCount - start) * resolution;
}
