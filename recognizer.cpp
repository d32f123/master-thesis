#include "recognizer.h"

#include <QtConcurrent>

Recognizer::Recognizer(QObject *parent) : QObject(parent), ctx(), socket(ctx, zmqpp::socket_type::sub) {}
const char* Recognizer::IPC_ENDPOINT = "ipc://recogsock";

void Recognizer::launch(const QVector<PatternModel> &patterns) {
    socket.subscribe("");
    socket.set(zmqpp::socket_option::receive_timeout, 1000);
    socket.bind(IPC_ENDPOINT);

    _shouldExit = new bool(false);
    bool* exit = _shouldExit;
    _poller = QtConcurrent::run([exit, this]() -> void {
        auto poller = _poller;
        while (!*exit) {
            zmqpp::message message;
            auto received = socket.receive(message);

            if (received) {
                std::string text;
                message >> text;
                emit valueFoundChanged(text == "1");
            }
            QThread::msleep(100);
        }
        delete exit;
    });

    // TODO: Launch python script

    setLaunched(true);
}

void Recognizer::stop() {
    *_shouldExit = true;

    // TODO: Stop python script

    setLaunched(false);
}

void Recognizer::setLaunched(bool v) {
    _isLaunched = v;
    emit launchedChanged(v);
}

bool Recognizer::launched() const {
    return _isLaunched;
}


