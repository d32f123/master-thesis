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

    static void savePattern(PatternModel& pattern);
    static void deletePattern(const PatternModel &pattern);
    static void renamePattern(PatternModel &pattern, const QString& new_name);
    static void savePatterns(QVector<PatternModel> &patterns);
    static void updatePattern(PatternModel &pattern);
    static std::optional<PatternModel> getPattern(const QString& name);
    static QVector<PatternModel> getPatterns();

    static PatternModel getFalsePattern();
protected:
    static QString patternPathFromName(const QString& pattern);
    static QString patternNameFromPath(const QString& pattern_path);
    static QString modelPathFromIdx(int idx);
    static QString recordingPathFromIdx(int idx);
    static int idxFromPath(const QString& recording_path);
};

#endif // PATTERNSERVICE_H
