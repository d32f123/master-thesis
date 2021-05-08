#ifndef PATTERNWINDOW_H
#define PATTERNWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include <QtMultimedia/QAudioOutput>

#include "patternmodel.h"
#include "iodevicerecorder.h"

namespace Ui {
    class PatternWindow;
}

class PatternWindow : public QDialog {
Q_OBJECT

public:
    explicit PatternWindow(PatternModel *model = nullptr, QWidget *parent = nullptr);

    ~PatternWindow();

    bool recording() const;
    bool recordingSelected() const;
    bool playback() const;
    bool editMode() const;
    bool nameValid() const;
    bool enoughRecordings() const;

    PatternModel *patternModel() const;

    void setRecording(bool v);
    void setRecordingSelected(bool v);
    void setPlayback(bool v);
    void setEditMode(bool v);
    void setNameValid(bool v);
    void setEnoughRecordings(bool v);
    void setSelectedRecording(QByteArray* v);

private slots:
    void on_createButton_clicked();
    void on_cancelButton_clicked();
    void on_patternNameField_textChanged(const QString &arg1);
    void on_toggleRecordingButton_clicked();
    void on_playRecordingButton_clicked();
    void on_PatternWindow_editModeChanged();
    void on_PatternWindow_recordingChanged();
    void on_recordingsListWidget_itemSelectionChanged();

    void checkPlayButton();
    void checkDeleteButton();
    void checkToggleButton();
    void checkCreateButton();
    void checkCancelButton();

    void on_deleteRecordingButton_clicked();

signals:
    void recordingChanged();
    void recordingSelectedChanged();
    void playbackChanged();
    void editModeChanged();
    void nameValidChanged();
    void enoughRecordingsChanged();

    void playButtonEnabledChanged(bool);
    void deleteButtonEnabledChanged(bool);
    void toggleButtonEnabledChanged(bool);
    void createButtonEnabledChanged(bool);
    void cancelButtonEnabledChanged(bool);

private:
    constexpr static int MIN_RECORDINGS_COUNT = 5;

    Ui::PatternWindow *ui;
    IODeviceRecorder *recorder;
    QAudioOutput *audioOutput;
    PatternModel *model;

    QListWidgetItem* add_recording(const QByteArray& recording);

    QList<QByteArray> _recordings;
    QByteArray* _selectedRecording = nullptr;

    bool isRecording = false;
    bool isRecordingSelected = false;
    bool isPlayback = false;
    bool isEditMode = false;
    bool isNameValid = false;
    bool isEnoughRecordings = false;
};

#endif // PATTERNWINDOW_H
