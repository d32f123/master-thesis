#include "recognizer.h"

#include <QProcess>

#include "patterns/patternservice.h"
#include "config/config.h"

Recognizer::Recognizer(QObject *parent) : QObject(parent) {}

void Recognizer::launch(const QVector<PatternModel> &patterns) {
    this->_recognizer = std::make_unique<recognizer>();
    connect(&*this->_recognizer, &recognizer::actionOccurred, this, &Recognizer::recognizerChanged,
            Qt::QueuedConnection);
    connect(&*_recognizer, &recognizer::done, this, &Recognizer::recognizerDone, Qt::QueuedConnection);

    this->_recognizer->start();
    setLaunched(true);
}

void Recognizer::stop() {
    this->_recognizer->exit();
}

void Recognizer::setLaunched(bool v) {
    _isLaunched = v;
    emit launchedChanged(v);
}

bool Recognizer::launched() const {
    return _isLaunched;
}

void Recognizer::recognizerChanged(bool v) {
    emit valueFoundChanged(v);
}

void Recognizer::recognizerDone() {
    setLaunched(false);
}

void recognizer::run() {
    dataSocket.subscribe("");
    dataSocket.set(zmqpp::socket_option::receive_timeout, 1000);
    dataSocket.bind(config::TWITRECOG_DATA_ENDPOINT);

    commandSocket.bind(config::TWITRECOG_COMMAND_ENDPOINT);

    qInfo("Launching recognizer");
    QProcess python_process{};
    python_process.start("python3", {
        config::getTwitRecogFilePath(),
        config::getRootDirectory().absolutePath()
    });

    if (python_process.state() == QProcess::ProcessState::NotRunning ||
        python_process.error() == QProcess::ProcessError::FailedToStart) {
        qWarning("Failed to start recognizer: %s", qUtf8Printable(python_process.errorString()));
        QByteArray std_out = python_process.readAll();
        if (!std_out.isEmpty()) qWarning("Recognizer output: %s", std_out.data());
        return;
    }

    qInfo("Listening for recognizer events");
    Worker::run();
    qInfo("Recognizer exiting");
    commandSocket.send("done");
    python_process.waitForFinished();
    QByteArray std_out = python_process.readAll();
    if (!std_out.isEmpty()) qWarning("Recognizer output: %s", std_out.data());
    dataSocket.close();
    commandSocket.close();

    emit done();
}

void recognizer::loop() {
    zmqpp::message message;
    auto received = dataSocket.receive(message);

    if (received) {
        std::string text;
        message >> text;
        emit actionOccurred(text == "true");
    }
    QThread::msleep(100);
}
