#ifndef PATTERNSERVICE_H
#define PATTERNSERVICE_H

#include <QVector>
#include <QString>

#include "patternmodel.h"


class PatternService {
public:
    PatternService();

    void addPattern(const PatternModel &pattern);

    void savePatterns(const QVector<PatternModel> &patterns);

    QVector<PatternModel> getPatterns();

protected:
    const static QString PATTERNS_FILENAME;
};

#endif // PATTERNSERVICE_H
