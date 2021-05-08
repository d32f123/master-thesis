#ifndef PATTERNMODEL_H
#define PATTERNMODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>

class PatternModel {
Q_GADGET;
public:
    PatternModel();

    PatternModel(const QString &name, bool active, QList<QByteArray> &&bytes);

    QString name() const;

    void setName(const QString &name);

    bool active() const;

    void setActive(bool active);

    QList<QByteArray>& recordings();
    const QList<QByteArray>& recordings() const;

    void setRecordings(QList<QByteArray> &&bytes);

private:
    QString _name;
    bool _active;
    QList<QByteArray> _recordings;
};

#endif // PATTERNMODEL_H
