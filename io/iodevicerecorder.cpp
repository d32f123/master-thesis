#include "iodevicerecorder.h"

#include <algorithm>

IODeviceRecorder::IODeviceRecorder(const QAudioDeviceInfo &deviceInfo, QObject* parent)
        : QObject(parent), buf(new QBuffer()) {
    audioInput = new QAudioInput(deviceInfo, defaultFormat(), this);
}

void IODeviceRecorder::start() {
    buf->open(QIODevice::ReadWrite);
    buf->seek(0);
    buf->buffer().reserve(sampleRate * 5);
    audioInput->start(buf);
}

void IODeviceRecorder::stop() {
    audioInput->stop();
    buf->seek(0);
}

void IODeviceRecorder::set(const QByteArray &arr, QIODevice::OpenMode mode) const {
    if (buf->isOpen()) {
        buf->close();
    }
    buf->setBuffer(nullptr);
    buf->open(mode);
    buf->write(arr);
    buf->seek(0);
}

void IODeviceRecorder::resetRead() const {
    buf->seek(0);
}

QAudioFormat IODeviceRecorder::defaultFormat() {
    QAudioDeviceInfo info {QAudioDeviceInfo::defaultOutputDevice()};

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(sizeof(float) * 8);
    audioFormat.setCodec("audio/pcm");
//    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(sampleType);

    if (!info.isFormatSupported(audioFormat)) {
        qWarning("Default format unsupported!");
        auto supportedSampleSizes = info.supportedSampleSizes();
        for (auto&& size : supportedSampleSizes) {
            qWarning("Supported sample size: %d", size);
        }
    }

    return audioFormat;
}
