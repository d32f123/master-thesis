#include "config.h"

#include <QStandardPaths>

const char *config::PATTERNS_DIR_NAME = "patterns";
const char *config::MODELS_DIR_NAME = "model";
const char *config::PYTHON_DIR_NAME = "python";
const char *config::PREPROCESS_DIR_NAME = "preprocessed";
const char *config::PREPROCESSOR_FILE_NAME = "model_updater.py";
const char *config::TWITRECOG_FILE_NAME = "twitrecog.py";
const char *config::PATTERN_PATH_SUFFIX = "_pattern";
const char *config::FALSE_PATTERN_NAME = "false";
const char *config::RECORDING_FILE_EXTENSION = ".raw";
const char *config::MODEL_FILE_EXTENSION = ".npy";
const char *config::MODEL_FILE_GLOB = "*.npy";
const char *config::RECORDING_FILE_GLOB = "*.raw";
const char *config::ACTIVE_FILE_NAME = "active.txt";
const char *config::TWITRECOG_DATA_ENDPOINT = "ipc://twitrecog_data";
const char *config::TWITRECOG_COMMAND_ENDPOINT = "ipc://twitrecog_command";


QDir config::getRootDirectory() {
    auto root_directory_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (root_directory_path.isEmpty())
        root_directory_path = ".";
    QDir dir = root_directory_path;
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    return dir;
}

QDir config::getPythonDirectory() {
    auto dir = QDir::current();
    dir.cd(PYTHON_DIR_NAME);
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    return dir;
}

QDir config::getModelsDirectory() {
    auto dir = getRootDirectory();
    dir.cd(MODELS_DIR_NAME);
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    return dir;
}

QDir config::getPatternsDirectory() {
    auto dir = getRootDirectory();
    dir.cd(PATTERNS_DIR_NAME);
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    return dir;
}

QDir config::getPreprocessDirectory() {
    auto dir = getRootDirectory();
    dir.cd(PREPROCESS_DIR_NAME);
    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }
    return dir;
}

QString config::getTwitRecogFilePath() {
    return getPythonDirectory().filePath(TWITRECOG_FILE_NAME);
}

QString config::getPreprocessorFilePath() {
    return getPythonDirectory().filePath(PREPROCESSOR_FILE_NAME);
}
