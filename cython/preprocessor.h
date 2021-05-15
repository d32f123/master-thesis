#ifndef TWITFIL_PREPROCESSOR_H
#define TWITFIL_PREPROCESSOR_H

#include <QThread>

class Preprocessor : public QThread {
    Q_OBJECT
public:
    explicit Preprocessor(QObject *parent = nullptr) : QThread(parent) {};
    void run() override;
};


#endif //TWITFIL_PREPROCESSOR_H
