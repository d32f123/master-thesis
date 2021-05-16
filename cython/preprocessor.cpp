#include "preprocessor.h"
#include "config/config.h"

#include <QProcess>

void Preprocessor::run() {
    QProcess python_process{};
    QStringList arguments {
            config::getPreprocessorFilePath(),
            config::getRootDirectory().absolutePath()
    };
    if (this->patternName) arguments.push_back(this->patternName.value());
    qInfo("Launching preprocessor %s", qUtf8Printable(arguments.last()));
    python_process.start("python3", arguments);

    if (python_process.state() == QProcess::ProcessState::NotRunning ||
        python_process.error() == QProcess::ProcessError::FailedToStart) {
        qWarning("Failed to start preprocessor: %s", qUtf8Printable(python_process.errorString()));
        QByteArray output = python_process.readAll();
        if (!output.isEmpty()) qWarning("Preprocessor output: %s", output.data());
        return;
    }
    python_process.waitForFinished();
    QByteArray output = python_process.readAll();
    if (!output.isEmpty()) qWarning("Preprocessor output: %s", output.data());
}
