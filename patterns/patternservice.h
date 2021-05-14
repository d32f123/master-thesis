#ifndef PATTERNSERVICE_H
#define PATTERNSERVICE_H

#include <QVector>
#include <QString>
#include <QDir>

#include <optional>

#include "patternmodel.h"


class PatternService {
public:
    PatternService();

    void addPattern(const PatternModel &pattern);
    void savePatterns(const QVector<PatternModel> &patterns);
    QVector<PatternModel> getPatterns();

    PatternModel getFalsePattern();
    void saveFalsePattern(const PatternModel& pattern);
protected:
    const static QString PATTERNS_DIRECTORY;
    const static QString PATTERN_PATH_SUFFIX;
    const static QString FALSE_PATTERN_NAME;
    const static QString RECORDING_FILE_EXTENSION;
    const static QString ACTIVE_FILE_NAME;
    const static QString MODELS_DIRECTORY;
    QDir patterns_directory;

    void savePattern(const PatternModel& pattern);
    std::optional<PatternModel> getPattern(const QString& pattern_dir);
    static QString patternPathFromName(const QString& pattern);
    static QString patternNameFromPath(const QString& pattern_path);
    static QString recordingPathFromIdx(int idx);
    static int recordingIdxFromPath(const QString& recording_path);
    static QString getRootDirectory();
};

#endif // PATTERNSERVICE_H
