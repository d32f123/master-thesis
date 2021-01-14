#ifndef DRIVERACTIONSPROVIDER_H
#define DRIVERACTIONSPROVIDER_H

#include <QObject>
#include <QFuture>

class DriverActionsProvider : public QObject
{
    Q_OBJECT
public:

    explicit DriverActionsProvider(QObject *parent = nullptr);
    ~DriverActionsProvider();

signals:
    void actionOccurred(bool actionType);

protected:
    bool* shouldExit;
    QFuture<void> poller;
    const static QString ACTIONS_FILENAME;
    bool latestAction = false;
};

#endif // DRIVERACTIONSPROVIDER_H
