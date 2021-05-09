#include "patternmodel.h"

PatternModel::PatternModel()
        : _name(), _active(false), _recordings() {

}

PatternModel::PatternModel(const QString &name, bool active, QVector<QByteArray> &&recordings)
        : _name(name), _active(active), _recordings(std::move(recordings)) {

}

void PatternModel::setName(const QString &name) {
    this->_name = name;
}

void PatternModel::setActive(bool active) {
    this->_active = active;
}

void PatternModel::setRecordings(QVector<QByteArray> &&recordings) {
    this->_recordings = recordings;
}

QString PatternModel::name() const {
    return this->_name;
}

bool PatternModel::active() const {
    return this->_active;
}

QVector<QByteArray>& PatternModel::recordings() {
    return this->_recordings;
}

const QVector<QByteArray>& PatternModel::recordings() const {
    return this->_recordings;
}
