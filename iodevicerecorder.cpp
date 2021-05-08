#include "iodevicerecorder.h"

#include <algorithm>

IODeviceRecorder::IODeviceRecorder(const QAudioDeviceInfo &deviceInfo)
        : buf(new QBuffer()) {
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

void IODeviceRecorder::set(const QByteArray &arr) {
    buf->open(QIODevice::ReadWrite);
    buf->write(arr);
    buf->seek(0);
}

void IODeviceRecorder::resetRead() {
    buf->seek(0);
}

QAudioFormat IODeviceRecorder::defaultFormat() {
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(8);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::UnSignedInt);

    return audioFormat;
}
