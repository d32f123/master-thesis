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
        series->replace(buf);
    }

    int start = 0;
    // 2 for transfer from char to float (1->4)
    int availableSamples = int(maxSize >> (2 + resolution_power));
    if (availableSamples < sampleCount) {
        std::move(buf.begin() + availableSamples, buf.end(), buf.begin());
        start = sampleCount - availableSamples;
    }

    const auto* float_data = reinterpret_cast<const float*>(data);
    for (int s = start; s < sampleCount; ++s, float_data += IODevicePlotter::resolution) {
        buf[s].setY(qreal(*float_data));
    }

    series->replace(buf);
//    qWarning("Max size: %d", maxSize);
    return maxSize;
}
