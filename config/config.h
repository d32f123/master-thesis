#ifndef TWITFIL_CONFIG_H
#define TWITFIL_CONFIG_H

#include <QString>
#include <QDir>

class config {
public:
    static QDir getRootDirectory();
    static QDir getPythonDirectory();
    static QDir getPatternsDirectory();
    static QDir getModelsDirectory();
    static QDir getPreprocessDirectory();

    static QString getTwitRecogFilePath();
    static QString getPreprocessorFilePath();

    const static char* PYTHON_DIR_NAME;
    const static char* PATTERNS_DIR_NAME;
    const static char* MODELS_DIR_NAME;
    const static char* PREPROCESS_DIR_NAME;
    const static char* PATTERN_PATH_SUFFIX;
    const static char* FALSE_PATTERN_NAME;
    const static char* RECORDING_FILE_EXTENSION;
    const static char* RECORDING_FILE_GLOB;
    const static char* MODEL_FILE_EXTENSION;
    const static char* MODEL_FILE_GLOB;
    const static char* ACTIVE_FILE_NAME;

    const static char* TWITRECOG_FILE_NAME;
    const static char* PREPROCESSOR_FILE_NAME;

    static const char* TWITRECOG_DATA_ENDPOINT;
    static const char* TWITRECOG_COMMAND_ENDPOINT;
};


#endif //TWITFIL_CONFIG_H
