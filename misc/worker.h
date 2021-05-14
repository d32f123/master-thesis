#ifndef TWITFIL_WORKER_H
#define TWITFIL_WORKER_H

#include <QThread>

class Worker : public QThread {
    Q_OBJECT
public:
    void exit() { this->shouldExit = true; };
protected:
    void run() override { while (!shouldExit) loop(); }
    virtual void loop() {};
    bool shouldExit = false;
signals:
    void actionOccurred(bool);
    void done();
};

#endif //TWITFIL_WORKER_H
