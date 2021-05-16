#ifndef PATTERNMODEL_H
#define PATTERNMODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>

struct PatternRecording {
    QByteArray bytes {};
    int order {-1};
};

class PatternModel {
Q_GADGET;
public:
    PatternModel();

    PatternModel(QString name, bool active, QVector<PatternRecording> &&recordings);

    [[nodiscard]] QString name() const;

    void setName(const QString &name);

    [[nodiscard]] bool active() const;

    void setActive(bool active);

    QVector<PatternRecording>& recordings();
    [[nodiscard]] const QVector<PatternRecording>& recordings() const;

    void setRecordings(QVector<PatternRecording> &&recordings);

private:
    QString _name;
    bool _active;
    QVector<PatternRecording> _recordings;
};

#endif // PATTERNMODEL_H
