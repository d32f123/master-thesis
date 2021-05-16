#include "patternmodel.h"

#include <utility>

PatternModel::PatternModel()
        : _name(), _active(false), _recordings() {}

PatternModel::PatternModel(QString name, bool active, QVector<PatternRecording> &&recordings)
        : _name(std::move(name)), _active(active), _recordings(std::move(recordings)) {}

void PatternModel::setName(const QString &name) {
    this->_name = name;
}

void PatternModel::setActive(bool active) {
    this->_active = active;
}

void PatternModel::setRecordings(QVector<PatternRecording> &&recordings) {
    this->_recordings = recordings;
}

QString PatternModel::name() const {
    return this->_name;
}

bool PatternModel::active() const {
    return this->_active;
}

QVector<PatternRecording>& PatternModel::recordings() {
    return this->_recordings;
}

const QVector<PatternRecording>& PatternModel::recordings() const {
    return this->_recordings;
}
