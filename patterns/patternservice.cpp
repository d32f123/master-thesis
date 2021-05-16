#include "patternservice.h"

#include <QtGlobal>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include "config/config.h"

void PatternService::addPattern(const PatternModel &pattern) {
    auto patterns = getPatterns();
    patterns.push_back(pattern);
    PatternService::savePatterns(patterns);
}

void PatternService::deletePattern(const PatternModel &pattern) {
    config::getModelsDirectory().rmdir(pattern.name());
    config::getPatternsDirectory().rmdir(pattern.name());
}

void PatternService::savePattern(const PatternModel &pattern) {
    auto pattern_dir_path = PatternService::patternPathFromName(pattern.name());
    auto dir = config::getPatternsDirectory();

    if (!dir.exists(pattern_dir_path)) {
        dir.mkdir(pattern_dir_path);
    }
    dir.cd(pattern_dir_path);

    for (auto&& entry : dir.entryList({config::RECORDING_FILE_GLOB}, QDir::Files)) {
        if (!dir.remove(entry)) {
            qWarning("Failed to remove file %s", qUtf8Printable(entry));
        }
    }
    {
        QFile active_file{dir.filePath(config::ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open active_file for writing: %s", qUtf8Printable(dir.filePath(config::ACTIVE_FILE_NAME)));
            return;
        }
        active_file.write(pattern.active() ? "true" : "false");
    }

    for (int i = 0; i < pattern.recordings().size(); ++i) {
        auto file_name = PatternService::recordingPathFromIdx(i);
        QFile recording_file{dir.filePath(file_name)};
        if (!recording_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open recording_file for writing: %s", qUtf8Printable(file_name));
            return;
        }

        recording_file.write(pattern.recordings()[i]);
    }
}

void PatternService::savePatterns(const QVector<PatternModel> &patterns) {
    for (auto &&pattern : patterns) savePattern(pattern);
}

std::optional<PatternModel> PatternService::getPattern(const QString &pattern_dir) {
    auto dir = config::getPatternsDirectory();

    PatternModel model;
    model.setName(PatternService::patternNameFromPath(pattern_dir));

    dir.cd(pattern_dir);
    {
        QFile active_file{dir.filePath(config::ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open active_file for reading for pattern %s", qUtf8Printable(model.name()));
            return {};
        }
        QString is_active = active_file.readAll();
        model.setActive(is_active == "true");
    }

    auto recordings_paths = dir.entryList({config::RECORDING_FILE_GLOB}, QDir::Files, QDir::Name);
    QVector<QByteArray> recordings;
    recordings.resize(recordings_paths.size());
    for (auto &&recording_path : recordings_paths) {
        auto path = dir.filePath(recording_path);
        auto idx = PatternService::recordingIdxFromPath(recording_path);

        QFile recording_file{path};
        if (!recording_file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open recording_file for reading: %s", qUtf8Printable(path));
            return {};
        }
        recordings[idx] = recording_file.readAll();
    }

    model.setRecordings(std::move(recordings));
    return model;
}

QVector<PatternModel> PatternService::getPatterns() {
    auto dir = config::getPatternsDirectory();
    auto patterns_dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    patterns_dirs.removeOne(PatternService::patternPathFromName(config::FALSE_PATTERN_NAME));

    QVector<PatternModel> ret{};
    for (auto &&pattern_dir_name : patterns_dirs) {
        auto pattern = getPattern(pattern_dir_name);
        if (!pattern) {
            qWarning("Error loading patterns while loading pattern %s", qUtf8Printable(pattern_dir_name));
            return ret;
        }
        ret.push_back(pattern.value());
    }

    return ret;
}

QString PatternService::patternPathFromName(const QString &pattern) {
    return pattern + config::PATTERN_PATH_SUFFIX;
}

QString PatternService::recordingPathFromIdx(int idx) {
    auto idx_str = std::to_string(idx);
    return QString(idx_str.c_str()) + config::RECORDING_FILE_EXTENSION;
}

QString PatternService::patternNameFromPath(const QString &pattern_path) {
    return pattern_path.section(config::PATTERN_PATH_SUFFIX, 0, 0);
}

int PatternService::recordingIdxFromPath(const QString &recording_path) {
    return recording_path.section(config::RECORDING_FILE_EXTENSION, 0, 0).toInt();
}

PatternModel PatternService::getFalsePattern() {
    if (auto pattern = getPattern(PatternService::patternPathFromName(config::FALSE_PATTERN_NAME)); pattern) {
        return pattern.value();
    }
    return {config::FALSE_PATTERN_NAME, true, {}};
}

void PatternService::saveFalsePattern(const PatternModel &pattern) {
    savePattern(pattern);
}


