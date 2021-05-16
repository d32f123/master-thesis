#ifndef PATTERNWINDOW_H
#define PATTERNWINDOW_H

#include <utility>

#include <QDialog>
#include <QListWidgetItem>
#include <QtMultimedia/QAudioOutput>

#include "patterns/patternmodel.h"
#include "io/iodevicerecorder.h"
#include "cython/preprocessor.h"

namespace Ui {
    class PatternWindow;
}

class PatternWindow : public QDialog {
Q_OBJECT

public:
    explicit PatternWindow(PatternModel *model = nullptr, QWidget *parent = nullptr);

    ~PatternWindow() override;

    [[nodiscard]] bool recording() const;
    [[nodiscard]] bool recordingSelected() const;
    [[nodiscard]] bool playback() const;
    [[nodiscard]] bool editMode() const;
    [[nodiscard]] bool nameValid() const;
    [[nodiscard]] bool enoughRecordings() const;
    [[nodiscard]] bool dirty() const;
    [[nodiscard]] bool renamed() const;
    [[nodiscard]] bool saving() const;
    [[nodiscard]] bool saved() const;

    [[nodiscard]] PatternModel *patternModel() const;

    void setRecording(bool v);
    void setRecordingSelected(bool v);
    void setPlayback(bool v);
    void setEditMode(bool v);
    void setNameValid(bool v);
    void setEnoughRecordings(bool v);
    void setSelectedRecording(PatternRecording* v);
    void setDirty(bool v);
    void setRenamed(bool v);
    void setSaving(bool v);
    void setSaved(bool v);

public slots:
    void onPreprocessingDone();

private slots:
    void onCreateButtonClicked();
    void onCancelButtonClicked();
    void onToggleRecordingButtonClicked();
    void onPlayRecordingButtonClicked();
    void onDeleteRecordingButtonClicked();
    void onPatternNameFieldTextChanged(const QString &arg1);
    void onRecordingsListWidgetItemSelectionChanged();

    void checkPlayButton();
    void checkDeleteButton();
    void checkToggleButton();
    void checkCreateButton();
    void checkCancelButton();

    void onDirtyChanged();
    void onEditModeChanged();
    void onRecordingChanged();

signals:
    void recordingChanged();
    void recordingSelectedChanged();
    void playbackChanged();
    void editModeChanged();
    void nameValidChanged();
    void enoughRecordingsChanged();
    void dirtyChanged();
    void renamedChanged();
    void savingChanged();
    void savedChanged();

    void playButtonEnabledChanged(bool);
    void deleteButtonEnabledChanged(bool);
    void toggleButtonEnabledChanged(bool);
    void createButtonEnabledChanged(bool);
    void cancelButtonEnabledChanged(bool);

private:
    constexpr static int MIN_RECORDINGS_COUNT = 3;

    Ui::PatternWindow *ui;
    std::unique_ptr<IODeviceRecorder> recorder;
    std::unique_ptr<Preprocessor> preprocessor;

    PatternModel *model;

    QListWidgetItem* add_recording(const PatternRecording& recording);

    QVector<PatternRecording> _recordings;
    PatternRecording* _selectedRecording = nullptr;

    void checkEnoughRecordings();
    void checkNameValid(const QString& s);

    void save();

    [[nodiscard]] QString getCreateButtonText() const;
    [[nodiscard]] QString getTitle() const;
    [[nodiscard]] QString getLabelText() const;

    bool isRecording = false;
    bool isRecordingSelected = false;
    bool isPlayback = false;
    bool isEditMode = false;
    bool isNameValid = false;
    bool isEnoughRecordings = false;
    bool isDirty = false;
    bool isRenamed = false;
    bool isSaving = false;
    bool isSaved = false;
};

#endif // PATTERNWINDOW_H
