#include "driveractionsprovider.h"

#include <QtConcurrentRun>
#include <QFile>

const QString DriverActionsProvider::ACTIONS_FILENAME =
#ifdef Q_OS_WIN
        "\\SystemRoot\\twitfilActions"
#else
        "/tmp/twitfil_actions"
#endif
;

DriverActionsProvider::DriverActionsProvider(QObject *parent) : QObject(parent) {
    shouldExit = new bool();
    *shouldExit = false;

    bool *exit = shouldExit;
    poller = QtConcurrent::run([exit, this]() -> void {
        QFile actionsFile{DriverActionsProvider::ACTIONS_FILENAME};
        if (!actionsFile.open(QIODevice::ReadWrite)) {
            qWarning("Could not open actions file for readWrite");
            return;
        }

        while (!*exit) {
            if (actionsFile.isReadable() && actionsFile.bytesAvailable() > 0) {
                QString str = actionsFile.readAll();
                qDebug() << "Read " << str << " from action file";
                QString lastLine = str.section('\n', -2, -2);

                bool currentAction = false;
                if (lastLine == "start")
                    currentAction = true;

                if (currentAction != this->latestAction) {
                    emit this->actionOccurred(currentAction);
                    this->latestAction = currentAction;
                }
            }
            QThread::msleep(300);
        }
    });
}

DriverActionsProvider::~DriverActionsProvider() {
    *this->shouldExit = true;
}
