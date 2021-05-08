#include "patternservice.h"

#include <qglobal.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

const QString PatternService::PATTERNS_FILENAME =
#ifdef Q_OS_WIN
        "\\SystemRoot\\patterns.json"
#else
        "/tmp/patterns.json"
#endif
;

PatternService::PatternService() {}

void PatternService::addPattern(const PatternModel &pattern) {
    auto patterns = this->getPatterns();
    patterns.push_back(pattern);
    this->savePatterns(patterns);
}

void PatternService::savePatterns(const QVector<PatternModel> &patterns) {
    QFile file{PatternService::PATTERNS_FILENAME};

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("Couldn't open patterns file for writing");
        return;
    }

    QJsonArray array;

    for (const auto &pattern : patterns) {
        QJsonObject obj;
        obj["name"] = pattern.name();
        obj["active"] = pattern.active();

        QJsonArray recordings{};
        for (const auto &recording : pattern.recordings()) {
            recordings.push_back(QString::fromUtf8(recording.toBase64()));
        }
        obj["recordings"] = recordings;

        array.push_back(obj);
    }

    file.write(QJsonDocument(array).toJson());
    file.close();
}

QVector<PatternModel> PatternService::getPatterns() {
    QFile file{PatternService::PATTERNS_FILENAME};

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open patterns file for reading");
        return {};
    }

    QByteArray patternsBytes{file.readAll()};
    file.close();

    QJsonDocument patternsDocument{QJsonDocument::fromJson(patternsBytes)};

    QJsonArray array{patternsDocument.array()};

    QVector<PatternModel> ret{};
    for (auto &&pattern : array) {
        QJsonObject modelObj = pattern.toObject();
        QList<QByteArray> recordings{};
        for (auto &&recording : modelObj["recordings"].toArray()) {
            recordings.push_back(QByteArray::fromBase64(recording.toString().toUtf8()));
        }
        PatternModel model{
                modelObj["name"].toString(),
                modelObj["active"].toBool(),
                std::move(recordings)
        };
        ret.push_back(model);
    }

    return ret;
}


