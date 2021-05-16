#include "patternservice.h"

#include <tuple>

#include <QSet>
#include <QtGlobal>
#include <QFile>
#include <QDir>

#include "config/config.h"

void PatternService::deletePattern(const PatternModel &pattern) {
    auto modelsDir = config::getModelsDirectory();
    auto patternsDir = config::getPatternsDirectory();
    auto preprocessDir = config::getPreprocessDirectory();
    modelsDir.cd(pattern.name());
    patternsDir.cd(patternPathFromName(pattern.name()));
    preprocessDir.cd(pattern.name());
    modelsDir.removeRecursively();
    patternsDir.removeRecursively();
    preprocessDir.removeRecursively();
}

void PatternService::renamePattern(PatternModel &pattern, const QString &new_name) {
    config::getModelsDirectory().rename(pattern.name(), new_name);
    config::getPatternsDirectory().rename(patternPathFromName(pattern.name()), patternPathFromName(new_name));
    config::getPreprocessDirectory().rename(pattern.name(), new_name);
    pattern.setName(new_name);
}

void PatternService::updatePattern(PatternModel &pattern) {
    pattern = getPattern(pattern.name()).value();
}

void PatternService::savePattern(PatternModel &pattern) {
    auto pattern_dir_path = PatternService::patternPathFromName(pattern.name());
    auto dir = config::getPatternsDirectory();
    auto preprocessedDir = config::getPreprocessDirectory();
    auto modelDir = config::getModelsDirectory();

    auto isNewPattern = false;
    if (!dir.exists(pattern_dir_path)) {
        dir.mkdir(pattern_dir_path);
        isNewPattern = true;
    }
    dir.cd(pattern_dir_path);
    preprocessedDir.cd(pattern.name());
    modelDir.cd(pattern.name());

    if (!isNewPattern) {
        QSet<int> ordering_set{};
        for (auto &&recording : pattern.recordings()) ordering_set.insert(recording.order);
        for (auto&&[d, l] : {
                std::make_tuple(std::ref(dir), dir.entryList({config::RECORDING_FILE_GLOB}, QDir::Files)),
                std::make_tuple(
                        std::ref(preprocessedDir),
                        preprocessedDir.entryList({config::RECORDING_FILE_GLOB}, QDir::Files)
                ),
                std::make_tuple(std::ref(modelDir), modelDir.entryList({"*"}, QDir::Files))
        }) {
            for (auto &&entry : l) {
                auto idx = idxFromPath(entry);
                if (!ordering_set.contains(idx) && !d.remove(entry))
                    qWarning("Failed to remove file %s", qUtf8Printable(entry));
            }
        }
    }

    {
        QFile active_file{dir.filePath(config::ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open active_file for writing: %s",
                     qUtf8Printable(dir.filePath(config::ACTIVE_FILE_NAME)));
            return;
        }
        active_file.write(pattern.active() ? "true" : "false");
    }

    // Move old files (order field) to new files (idx in array)
    for (int i = 0; i < pattern.recordings().count(); ++i) {
        auto &&recording = pattern.recordings()[i];
        if (recording.order == -1 || recording.order == i) continue;
        auto old_name = recordingPathFromIdx(recording.order);
        auto new_name = recordingPathFromIdx(i);
        if (!dir.rename(old_name, new_name))
            qWarning("Failed to rename %s to %s", qUtf8Printable(old_name), qUtf8Printable(new_name));
        if (!preprocessedDir.rename(old_name, new_name))
            qWarning("Failed to rename %s to %s", qUtf8Printable(old_name), qUtf8Printable(new_name));
        if (!modelDir.rename(modelPathFromIdx(recording.order), modelPathFromIdx(i)))
            qWarning("Failed to rename %s to %s",
                     qUtf8Printable(modelPathFromIdx(recording.order)), qUtf8Printable(modelPathFromIdx(i)));
    }
    //  Create new files with name as idx in array, assign idx in array as order
    for (int i = 0; i < pattern.recordings().size(); ++i) {
        auto &&recording = pattern.recordings()[i];
        if (recording.order != -1) continue;
        recording.order = i;

        auto file_name = PatternService::recordingPathFromIdx(i);
        QFile recording_file{dir.filePath(file_name)};
        if (!recording_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open recording_file for writing: %s", qUtf8Printable(file_name));
            return;
        }

        recording_file.write(pattern.recordings()[i].bytes);
    }
}

void PatternService::savePatterns(QVector<PatternModel> &patterns) {
    for (auto &&pattern : patterns) savePattern(pattern);
}

std::optional<PatternModel> PatternService::getPattern(const QString &name) {
    auto dir = config::getPatternsDirectory();
    auto preprocessedDir = config::getPreprocessDirectory();
    auto pattern_dir = patternPathFromName(name);

    PatternModel model;
    model.setName(name);

    dir.cd(pattern_dir);
    preprocessedDir.cd(name);
    {
        QFile active_file{dir.filePath(config::ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open active_file for reading for pattern %s", qUtf8Printable(name));
            return {};
        }
        QString is_active = active_file.readAll();
        model.setActive(is_active == "true");
    }

    auto recordings_paths = dir.entryList({config::RECORDING_FILE_GLOB}, QDir::Files, QDir::Name);
    QVector<PatternRecording> recordings;
    recordings.resize(recordings_paths.count());
    for (auto &&recording_path : recordings_paths) {
        auto path = dir.filePath(recording_path);
        auto preprocessedPath = preprocessedDir.filePath(recording_path);
        auto idx = idxFromPath(recording_path);

        QFile recording_file{preprocessedPath};
        if (!recording_file.open(QIODevice::ReadOnly)) {
            recording_file.setFileName(path);
            if (!recording_file.open(QIODevice::ReadOnly)) {
                qWarning("Couldn't open recording_file for reading: %s", qUtf8Printable(path));
                return {};
            }
        }
        recordings[idx] = {.bytes = recording_file.readAll(), .order = idx};
    }

    model.setRecordings(std::move(recordings));
    return model;
}

QVector<PatternModel> PatternService::getPatterns() {
    auto dir = config::getPatternsDirectory();
    auto patterns_dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    patterns_dirs.removeOne(patternPathFromName(config::FALSE_PATTERN_NAME));

    QVector<PatternModel> ret{};
    for (auto &&pattern_dir_name : patterns_dirs) {
        auto pattern = getPattern(patternNameFromPath(pattern_dir_name));
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

int PatternService::idxFromPath(const QString &recording_path) {
    return recording_path.section(".", 0, 0).toInt();
}

QString PatternService::modelPathFromIdx(int idx) {
    auto idx_str = std::to_string(idx);
    return QString(idx_str.c_str()) + config::MODEL_FILE_EXTENSION;
}

PatternModel PatternService::getFalsePattern() {
    if (auto pattern = getPattern(config::FALSE_PATTERN_NAME); pattern) {
        return pattern.value();
    }
    return {config::FALSE_PATTERN_NAME, true, {}};
}
