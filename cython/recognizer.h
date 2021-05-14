#ifndef TWITFIL_RECOGNIZER_H
#define TWITFIL_RECOGNIZER_H

#include <utility>

#include <QVector>
#include <QFuture>
#include <QThread>
#include <QProcess>

#include "patterns/patternmodel.h"

#include <zmqpp/zmqpp.hpp>

#include "misc/worker.h"

class recognizer : public Worker {
Q_OBJECT
protected:
    static const char* IPC_ENDPOINT;
    static const char* DONE_ENDPOINT;

    void run() override;
    void loop() override;

    zmqpp::context ctx {};
    zmqpp::socket socket {ctx, zmqpp::socket_type::sub};
    zmqpp::socket doneSocket {ctx, zmqpp::socket_type::pub};
};

class Recognizer : public QObject {
    Q_OBJECT
public:
    explicit Recognizer(QObject *parent = nullptr);

    void launch(const QVector<PatternModel>& patterns);
    void stop();

    void setLaunched(bool v);
    bool launched() const;
signals:
    void valueFoundChanged(bool);
    void launchedChanged(bool);

private slots:
    void recognizerChanged(bool v);
    void recognizerDone();

protected:
    std::unique_ptr<recognizer> _recognizer = nullptr;
    bool _isLaunched = false;
};


#endif //TWITFIL_RECOGNIZER_H
