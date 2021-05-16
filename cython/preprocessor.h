#ifndef TWITFIL_PREPROCESSOR_H
#define TWITFIL_PREPROCESSOR_H

#include <optional>

#include <QThread>

class Preprocessor : public QThread {
    Q_OBJECT
public:
    explicit Preprocessor(QObject *parent = nullptr) : QThread(parent) {};

    std::optional<QString> patternName;

    void run() override;
};


#endif //TWITFIL_PREPROCESSOR_H
