#include "patternservice.h"

#include <QtGlobal>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

const QString PatternService::PATTERNS_DIRECTORY = "patterns";
const QString PatternService::PATTERN_PATH_SUFFIX = "_pattern";
const QString PatternService::FALSE_PATTERN_NAME = "false";
const QString PatternService::RECORDING_FILE_EXTENSION = ".pcm";
const QString PatternService::ACTIVE_FILE_NAME = "active.txt";

PatternService::PatternService() {
    QDir dir = PatternService::getRootDirectory();
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    if (!dir.exists(PATTERNS_DIRECTORY)) {
        dir.mkdir(PATTERNS_DIRECTORY);
    }
    dir.cd(PATTERNS_DIRECTORY);
    qInfo("Directory path: %s", qUtf8Printable(dir.absolutePath()));
    this->patterns_directory = dir;
}

void PatternService::addPattern(const PatternModel &pattern) {
    auto patterns = this->getPatterns();
    patterns.push_back(pattern);
    PatternService::savePatterns(patterns);
}

void PatternService::savePattern(const PatternModel &pattern) {
    auto pattern_dir_path = PatternService::patternPathFromName(pattern.name());
    auto dir = patterns_directory;

    if (!dir.exists(pattern_dir_path)) {
        dir.mkdir(pattern_dir_path);
    }
    dir.cd(pattern_dir_path);

    {
        QFile active_file{dir.filePath(ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open active_file for writing");
            return;
        }
        active_file.write(pattern.active() ? "true" : "false");
    }

    for (int i = 0; i < pattern.recordings().size(); ++i) {
        QFile recording_file{dir.filePath(PatternService::recordingPathFromIdx(i))};
        if (!recording_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("Couldn't open recording_file for writing");
            return;
        }

        recording_file.write(pattern.recordings()[i]);
    }
}

void PatternService::savePatterns(const QVector<PatternModel> &patterns) {
    for (auto&& pattern : patterns) savePattern(pattern);
}

std::optional<PatternModel> PatternService::getPattern(const QString &pattern_dir) {
    auto dir = patterns_directory;

    PatternModel model;
    model.setName(PatternService::patternNameFromPath(pattern_dir));
    qInfo("Loading pattern %s from %s", qUtf8Printable(model.name()), qUtf8Printable(pattern_dir));

    dir.cd(pattern_dir);
    {
        QFile active_file{dir.filePath(ACTIVE_FILE_NAME)};
        if (!active_file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open active_file for reading");
            return {};
        }
        QString is_active = active_file.readAll();
        model.setActive(is_active == "true");
    }

    auto recordings_paths = dir.entryList(QStringList("*" + RECORDING_FILE_EXTENSION), QDir::Files, QDir::Name);
    QVector<QByteArray> recordings;
    recordings.resize(recordings_paths.size());
    for (auto&& recording_path : recordings_paths) {
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
    auto dir = patterns_directory;
    auto patterns_dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    patterns_dirs.removeOne(PatternService::patternPathFromName(FALSE_PATTERN_NAME));

    QVector<PatternModel> ret{};
    for (auto&& pattern_dir_name : patterns_dirs) {
        qInfo("Loading pattern %s", qUtf8Printable(pattern_dir_name));
        auto pattern = getPattern(pattern_dir_name);
        if (!pattern) {
            qWarning("Error loading patterns");
            return ret;
        }
        ret.push_back(pattern.value());
    }

    return ret;
}

QString PatternService::patternPathFromName(const QString &pattern) {
    return pattern + PATTERN_PATH_SUFFIX;
}

QString PatternService::recordingPathFromIdx(int idx) {
    auto idx_str = std::to_string(idx);
    return QString(idx_str.c_str()) + RECORDING_FILE_EXTENSION;
}

QString PatternService::getRootDirectory() {
    auto root_directory_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (root_directory_path.isEmpty()) root_directory_path = ".";
    return root_directory_path;
}

QString PatternService::patternNameFromPath(const QString &pattern_path) {
    return pattern_path.section(PATTERN_PATH_SUFFIX, 0, 0);
}

int PatternService::recordingIdxFromPath(const QString &recording_path) {
    return recording_path.section(RECORDING_FILE_EXTENSION, 0, 0).toInt();
}

std::optional<PatternModel> PatternService::getFalsePattern() {
    return getPattern(PatternService::patternPathFromName(FALSE_PATTERN_NAME));
}

void PatternService::saveFalsePattern(const PatternModel &pattern) {
    savePattern(pattern);
}


