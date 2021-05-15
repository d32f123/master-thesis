#include "preprocessor.h"
#include "config/config.h"

#include <QProcess>

void Preprocessor::run() {
    QProcess python_process{};
    qInfo("Launching preprocessor");
    python_process.start("python3", {
            config::getPreprocessorFilePath(),
            config::getRootDirectory().absolutePath()
    });

    if (python_process.state() == QProcess::ProcessState::NotRunning ||
        python_process.error() == QProcess::ProcessError::FailedToStart) {
        qWarning("Failed to start python: %s", qUtf8Printable(python_process.errorString()));
        QByteArray std_out = python_process.readAll();
        qWarning("Std out: %s", std_out.data());
        return;
    }
    python_process.waitForFinished();
    QByteArray std_out = python_process.readAll();
    qWarning("Std out: %s", std_out.data());
}
