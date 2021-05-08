#include "patternmodel.h"

PatternModel::PatternModel()
        : _name(), _active(false), _bytes() {

}

PatternModel::PatternModel(const QString &name, bool active, QByteArray &&bytes)
        : _name(name), _active(active), _bytes(std::move(bytes)) {

}

void PatternModel::setName(const QString &name) {
    this->_name = name;
}

void PatternModel::setActive(bool active) {
    this->_active = active;
}

void PatternModel::setBytes(QByteArray &&bytes) {
    this->_bytes = bytes;
}

QString PatternModel::name() const {
    return this->_name;
}

bool PatternModel::active() const {
    return this->_active;
}

QByteArray PatternModel::bytes() const {
    return this->_bytes;
}
