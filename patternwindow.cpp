#include "patternwindow.h"
#include "ui_patternwindow.h"

#include <QtMultimedia/QAudioOutput>

#include "iodevicerecorder.h"

PatternWindow::PatternWindow(PatternModel *model, QWidget *parent) :
    QDialog(parent, Qt::Dialog),
    ui(new Ui::PatternWindow),
    recorder(new IODeviceRecorder(QAudioDeviceInfo::defaultInputDevice())),
    model(model)
{
    ui->setupUi(this);

    if (!model) {
        this->model = model = new PatternModel();
    } else {
        setRecorded(true);
        setEditMode(true);
    }

    ui->ioPlotter->initialize(QAudioDeviceInfo::defaultInputDevice());
    ui->patternNameField->setText(model->name());

    ui->ioPlotter->setAudio(model->bytes());
    recorder->set(model->bytes());
}

PatternWindow::~PatternWindow()
{
    delete ui;
}

bool PatternWindow::recording() const
{
    return isRecording;
}

bool PatternWindow::recorded() const
{
    return isRecorded;
}

bool PatternWindow::playback() const
{
    return isPlayback;
}

bool PatternWindow::editMode() const
{
    return isEditMode;
}

PatternModel *PatternWindow::patternModel() const
{
    return model;
}

void PatternWindow::setRecording(bool v)
{
    isRecording = v;
    emit recordingChanged(v);
}

void PatternWindow::setRecorded(bool v)
{
    isRecorded = v;
    emit recordedChanged(v);
}

void PatternWindow::setPlayback(bool v)
{
    isPlayback = v;
    emit playbackChanged(v);
}

void PatternWindow::setEditMode(bool v)
{
    isEditMode = v;
    emit editModeChanged(v);
}

void PatternWindow::on_createButton_clicked()
{
    model->setName(ui->patternNameField->text());
    model->setBytes(QByteArray(recorder->buf->buffer()));
    accept();
}

void PatternWindow::on_cancelButton_clicked()
{
    reject();
}

void PatternWindow::on_patternNameField_textChanged(const QString &)
{
    checkValidity();
}

void PatternWindow::checkValidity()
{
    ui->createButton->setDisabled(ui->patternNameField->text().isEmpty() || !isRecorded);
}

void PatternWindow::on_toggleRecordingButton_clicked()
{
    if (!isRecording) {
        recorder->start();
        ui->ioPlotter->start();
        setRecording(true);
        setRecorded(false);
    } else {
        recorder->stop();
        ui->ioPlotter->stop();
        setRecording(false);
        setRecorded(true);
    }

}

void PatternWindow::on_playRecordingButton_clicked()
{
    QAudioDeviceInfo audioInfo = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat format = IODeviceRecorder::defaultFormat();
    if (!audioInfo.isFormatSupported(format)) {
        qWarning("Default format is unsupported for playback");
        return;
    }

    audioOutput = new QAudioOutput(format, this);
    connect(audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State state) -> void {
        switch (state) {
        case QAudio::IdleState:
            audioOutput->stop();
            delete audioOutput;
            setPlayback(false);
            break;
        case QAudio::StoppedState:
            if (audioOutput->error() != QAudio::NoError) {
                qWarning("Error during playback: %d", audioOutput->error());
            }
            setPlayback(false);
            break;
        default:
            break;
        }
    });
    recorder->resetRead();
    audioOutput->start(recorder->buf);
    setPlayback(true);
}

void PatternWindow::on_PatternWindow_editModeChanged(bool isEdit)
{
    if (isEdit) {
        this->setWindowTitle("Edit Pattern");
        this->ui->newPatternLabel->setText("Edit an existing Pattern");
        this->ui->createButton->setText("Save");
    } else {
        this->setWindowTitle("Create Pattern");
        this->ui->newPatternLabel->setText("Create a new Pattern");
        this->ui->createButton->setText("Create");
    }
}

void PatternWindow::on_PatternWindow_recordingChanged(bool rec)
{
    this->ui->toggleRecordingButton->setText(rec ? "Stop Recording" : "Start Recording");
}

void PatternWindow::on_PatternWindow_recordedChanged(bool )
{
    checkValidity();

}
