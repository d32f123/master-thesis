#ifndef PATTERNMODEL_H
#define PATTERNMODEL_H

#include <QObject>
#include <QString>
#include <QVector>

class PatternModel {
Q_GADGET;
public:
    PatternModel();

    PatternModel(const QString &name, bool active, QVector<QByteArray> &&bytes);

    QString name() const;

    void setName(const QString &name);

    bool active() const;

    void setActive(bool active);

    QVector<QByteArray>& recordings();
    const QVector<QByteArray>& recordings() const;

    void setRecordings(QVector<QByteArray> &&bytes);

private:
    QString _name;
    bool _active;
    QVector<QByteArray> _recordings;
};

#endif // PATTERNMODEL_H
