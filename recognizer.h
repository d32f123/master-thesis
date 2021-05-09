#ifndef TWITFIL_RECOGNIZER_H
#define TWITFIL_RECOGNIZER_H

#include <QVector>
#include <QFuture>
#include "patternmodel.h"

#include <zmqpp/zmqpp.hpp>

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

protected:
    static const char* IPC_ENDPOINT;

    bool *_shouldExit = nullptr;
    QFuture<void> _poller;
    bool _isLaunched = false;
    zmqpp::context ctx;
    zmqpp::socket socket;
};


#endif //TWITFIL_RECOGNIZER_H
