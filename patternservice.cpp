#include "patternservice.h"

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

PatternService::PatternService()
{}

void PatternService::addPattern(const PatternModel &pattern)
{
    auto patterns = this->getPatterns();
    patterns.push_back(pattern);
    this->savePatterns(patterns);
}

void PatternService::savePatterns(const QVector<PatternModel>& patterns)
{
    QFile file{PatternService::PATTERNS_FILENAME};

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("Couldn't open patterns file for writing");
        return;
    }

    QJsonArray array;

    for (int i = 0; i < patterns.size(); ++i) {
        QJsonObject obj;
        const PatternModel& pattern = patterns[i];
        obj["name"] = pattern.name();
        obj["active"] = pattern.active();
        obj["bytes"] = QString::fromUtf8(pattern.bytes().toBase64());
        array.push_back(obj);
    }

    file.write(QJsonDocument(array).toJson());
    file.close();
}

QVector<PatternModel> PatternService::getPatterns()
{
    QFile file {PatternService::PATTERNS_FILENAME};

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open patterns file for reading");
        return {};
    }

    QByteArray patternsBytes {file.readAll()};
    file.close();

    QJsonDocument patternsDocument {QJsonDocument::fromJson(patternsBytes)};

    QJsonArray array {patternsDocument.array()};

    QVector<PatternModel> ret {};
    for (int i = 0; i < array.size(); ++i) {
        QJsonObject modelObj = array[i].toObject();
        PatternModel model {
            modelObj["name"].toString(),
            modelObj["active"].toBool(),
            QByteArray::fromBase64(modelObj["bytes"].toString().toUtf8())
        };
        ret.push_back(model);
    }

    return ret;
}


