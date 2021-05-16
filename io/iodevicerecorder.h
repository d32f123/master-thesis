#ifndef IODEVICERECORDER_H
#define IODEVICERECORDER_H

#include <QIODevice>
#include <QObject>
#include <QBuffer>

#include <QtMultimedia/QAudioInput>

class IODeviceRecorder : public QObject {
Q_OBJECT
public:
    explicit IODeviceRecorder(const QAudioDeviceInfo &deviceInfo, QObject* parent = nullptr);

    QBuffer *buf;

    void start();

    void stop();

    void set(const QByteArray &arr, QIODevice::OpenMode mode = QIODevice::ReadWrite) const;

    void resetRead() const;

    static QAudioFormat defaultFormat();

    static const int sampleRate = 22050;
    static const QAudioFormat::SampleType sampleType = QAudioFormat::SampleType::Float;

private:
    QAudioInput *audioInput;
};

#endif // IODEVICERECORDER_H
