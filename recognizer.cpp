#include "recognizer.h"

#include <condition_variable>
#include <QtConcurrent>

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
    std::mutex m{};
    std::condition_variable cv {};
    QtConcurrent::run([&m, &cv]() {
        qInfo("python starting");
        auto programName = std::string("recog");
        std::string path = "twitrecog.py";
        FILE *p = fopen(path.c_str(), "rb");
        if (!p) {
            qWarning("Failed to open twitrecog.py");
            return;
        }

        Py_SetProgramName(programName.data());
        Py_Initialize();
        PyRun_SimpleFile(p, path.c_str());
        fclose(p);

        {
            std::unique_lock l{m};
            cv.notify_all();
        }

        qInfo("Python exiting");
    });
    qInfo("Recognizer loop");
    Worker::run();
    {
        std::unique_lock l{m};
        qInfo("Recognizer exiting");
        doneSocket.send("done");
        cv.wait(l);
        socket.close();
        doneSocket.close();
    }

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
