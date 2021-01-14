#ifndef PATTERNMODEL_H
#define PATTERNMODEL_H
#include <QObject>
#include <QString>
#include <QVector>

class PatternModel
{
    Q_GADGET;
public:
    PatternModel();
    PatternModel(const QString& name, bool active, QByteArray&& bytes);

    QString name() const;
    void setName(const QString& name);

    bool active() const;
    void setActive(bool active);

    QByteArray bytes() const;
    void setBytes(QByteArray&& bytes);
private:
    QString _name;
    bool _active;
    QByteArray _bytes;
};

#endif // PATTERNMODEL_H
