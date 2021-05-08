#ifndef IODEVICERECORDER_H
#define IODEVICERECORDER_H

#include <QIODevice>
#include <QObject>
#include <QBuffer>

#include <QtMultimedia/QAudioInput>

class IODeviceRecorder : public QObject {
Q_OBJECT
public:
    IODeviceRecorder(const QAudioDeviceInfo &deviceInfo);

    QBuffer *buf;

    void start();

    void stop();

    void set(const QByteArray &arr, QIODevice::OpenMode mode = QIODevice::ReadWrite);

    void resetRead();

    static QAudioFormat defaultFormat();

    static const int sampleRate = 44100;

private:
    QAudioInput *audioInput;
};

#endif // IODEVICERECORDER_H
