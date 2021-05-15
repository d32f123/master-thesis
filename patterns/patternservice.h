#ifndef PATTERNSERVICE_H
#define PATTERNSERVICE_H

#include <QVector>
#include <QString>
#include <QDir>

#include <optional>

#include "patternmodel.h"

class PatternService {
public:
    PatternService() = delete;

    static void addPattern(const PatternModel &pattern);
    static void deletePattern(const PatternModel &pattern);
    static void savePatterns(const QVector<PatternModel> &patterns);
    static QVector<PatternModel> getPatterns();

    static PatternModel getFalsePattern();
    static void saveFalsePattern(const PatternModel& pattern);
protected:
    static void savePattern(const PatternModel& pattern);
    static std::optional<PatternModel> getPattern(const QString& pattern_dir);
    static QString patternPathFromName(const QString& pattern);
    static QString patternNameFromPath(const QString& pattern_path);
    static QString recordingPathFromIdx(int idx);
    static int recordingIdxFromPath(const QString& recording_path);
};

#endif // PATTERNSERVICE_H
