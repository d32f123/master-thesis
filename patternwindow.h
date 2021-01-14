#ifndef PATTERNWINDOW_H
#define PATTERNWINDOW_H

#include <QDialog>
#include <QtMultimedia/QAudioOutput>

#include "patternmodel.h"
#include "iodevicerecorder.h"

namespace Ui {
class PatternWindow;
}

class PatternWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PatternWindow(PatternModel* model, QWidget *parent = nullptr);
    ~PatternWindow();

    bool recording() const;
    bool recorded() const;
    bool playback() const;
    bool editMode() const;
    PatternModel *patternModel() const;
    void setRecording(bool v);
    void setRecorded(bool v);
    void setPlayback(bool v);
    void setEditMode(bool v);

private slots:
    void on_createButton_clicked();
    void on_cancelButton_clicked();
    void on_patternNameField_textChanged(const QString &arg1);
    void on_toggleRecordingButton_clicked();
    void on_playRecordingButton_clicked();
    void on_PatternWindow_editModeChanged(bool );
    void on_PatternWindow_recordingChanged(bool );
    void on_PatternWindow_recordedChanged(bool );

signals:
    void recordingChanged(bool v);
    void recordedChanged(bool v);
    void playbackChanged(bool v);
    void editModeChanged(bool v);

private:
    Ui::PatternWindow *ui;
    IODeviceRecorder* recorder;
    QAudioOutput* audioOutput;
    PatternModel* model;
    void checkValidity();

    bool isRecording = false;
    bool isRecorded = false;
    bool isPlayback = false;
    bool isEditMode = false;

};

#endif // PATTERNWINDOW_H
