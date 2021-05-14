#include "recognizer.h"

#include <QtConcurrent>
#include <QProcess>

#define PY_SSIZE_T_CLEAN

#include <Python/Python.h>

Recognizer::Recognizer(QObject *parent) : QObject(parent) {}

const char *recognizer::IPC_ENDPOINT = "ipc://recogsock";
const char *recognizer::DONE_ENDPOINT = "ipc://recogsockdone";

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
    qInfo("Recognizer starting");
    socket.subscribe("");
    socket.set(zmqpp::socket_option::receive_timeout, 1000);
    socket.bind(IPC_ENDPOINT);

    doneSocket.bind(DONE_ENDPOINT);

    qInfo("Launching python");
    QProcess python_process{};
    python_process.start("python3", {"twitrecog.py"});

    if (python_process.state() == QProcess::ProcessState::NotRunning ||
        python_process.error() == QProcess::ProcessError::FailedToStart) {
        qWarning("Failed to start python: %s", qUtf8Printable(python_process.errorString()));
        QByteArray std_out = python_process.readAll();
        qWarning("Std out: %s", std_out.data());
        return;
    }

    qInfo("Launched python");
    qInfo("Recognizer loop");
    Worker::run();
    qInfo("Recognizer exiting");
    doneSocket.send("done");
    python_process.waitForFinished();
    QByteArray std_out = python_process.readAll();
    qWarning("Std out: %s", std_out.data());
    socket.close();
    doneSocket.close();

    emit done();
}

void recognizer::loop() {
    zmqpp::message message;
    auto received = socket.receive(message);

    if (received) {
        std::string text;
        message >> text;
        emit actionOccurred(text == "true");
    }
    QThread::msleep(100);
}
