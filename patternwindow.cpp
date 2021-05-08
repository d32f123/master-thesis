#include "patternwindow.h"
#include "ui_patternwindow.h"

#include <QtMultimedia/QAudioOutput>

#include "iodevicerecorder.h"

PatternWindow::PatternWindow(PatternModel *model, QWidget *parent) :
        QDialog(parent, Qt::Dialog),
        ui(new Ui::PatternWindow),
        recorder(new IODeviceRecorder(QAudioDeviceInfo::defaultInputDevice())),
        model(model) {
    ui->setupUi(this);

    setEditMode(model);
    if (!model) {
        this->model = model = new PatternModel();
    }

    ui->ioPlotter->initialize(QAudioDeviceInfo::defaultInputDevice());
    ui->patternNameField->setText(model->name());

    QListWidgetItem* first = nullptr;
    for (auto&& recording : model->recordings()) {
        auto item = add_recording(recording);
        if (first == nullptr) first = item;
    }
    if (first) ui->recordingsListWidget->setCurrentItem(first);

    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkDeleteButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkCreateButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkCancelButton);

    connect(this, &PatternWindow::recordingSelectedChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::recordingSelectedChanged, this, &PatternWindow::checkDeleteButton);

    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkDeleteButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkToggleButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkCreateButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkCancelButton);

    connect(this, &PatternWindow::nameValidChanged, this, &PatternWindow::checkCreateButton);

    connect(this, &PatternWindow::enoughRecordingsChanged, this, &PatternWindow::checkCreateButton);
}

PatternWindow::~PatternWindow() {
    delete ui;
}

bool PatternWindow::recording() const {
    return isRecording;
}

bool PatternWindow::recordingSelected() const {
    return isRecordingSelected;
}

bool PatternWindow::playback() const {
    return isPlayback;
}

bool PatternWindow::editMode() const {
    return isEditMode;
}

bool PatternWindow::nameValid() const {
    return isNameValid;
}

bool PatternWindow::enoughRecordings() const {
    return isEnoughRecordings;
}

PatternModel *PatternWindow::patternModel() const {
    return model;
}

void PatternWindow::setRecording(bool v) {
    isRecording = v;
    emit recordingChanged();
}

void PatternWindow::setRecordingSelected(bool v) {
    isRecordingSelected = v;
    emit recordingSelectedChanged();
}

void PatternWindow::setPlayback(bool v) {
    isPlayback = v;
    emit playbackChanged();
}

void PatternWindow::setEditMode(bool v) {
    isEditMode = v;
    emit editModeChanged();
}

void PatternWindow::setNameValid(bool v) {
    isNameValid = v;
    emit nameValidChanged();
}

void PatternWindow::setEnoughRecordings(bool v) {
    isEnoughRecordings = v;
    emit enoughRecordingsChanged();
}

void PatternWindow::setSelectedRecording(QByteArray *v) {
    _selectedRecording = v;
    ui->ioPlotter->setAudio(v ? *v : QByteArray());
    recorder->set(v ? *v : QByteArray());
}

void PatternWindow::on_createButton_clicked() {
    model->setName(ui->patternNameField->text());
    model->setRecordings(std::move(_recordings));
    accept();
}

void PatternWindow::on_cancelButton_clicked() {
    reject();
}

void PatternWindow::on_patternNameField_textChanged(const QString &s) {
    setNameValid(!s.isEmpty());
}

QListWidgetItem* PatternWindow::add_recording(const QByteArray &recording) {
    _recordings.push_back(recording);
    int idx = _recordings.size() - 1;

    QString name = "Recording ";
    name.append(std::to_string(idx + 1).c_str());
    auto* item = new QListWidgetItem{name};
    item->setData(Qt::UserRole, idx);

    ui->recordingsListWidget->addItem(item);
    return item;
}

void PatternWindow::on_toggleRecordingButton_clicked() {
    if (!isRecording) {
        recorder->set(QByteArray());
        recorder->start();
        ui->ioPlotter->start();
        setRecording(true);
    } else {
        recorder->stop();
        ui->ioPlotter->stop();
        setRecording(false);

        auto item = add_recording(recorder->buf->buffer());
        ui->recordingsListWidget->setCurrentItem(item);

        bool enoughRecordings = _recordings.size() >= PatternWindow::MIN_RECORDINGS_COUNT;
        if (enoughRecordings != this->enoughRecordings()) {
            setEnoughRecordings(enoughRecordings);
        }
    }
}

void PatternWindow::on_deleteRecordingButton_clicked()
{
    auto item = ui->recordingsListWidget->selectedItems().first();
    auto idx = item->data(Qt::UserRole).toInt();
    for (auto i = idx + 1; i < ui->recordingsListWidget->count(); ++i) {
        ui->recordingsListWidget->item(i)->setData(Qt::UserRole, i - 1);
        ui->recordingsListWidget->item(i)->setText(QString(std::string("Recording " + std::to_string(i)).c_str()));
    }
    _recordings.removeAt(idx);
    delete item;

    bool enoughRecordings = _recordings.size() >= PatternWindow::MIN_RECORDINGS_COUNT;
    if (enoughRecordings != this->enoughRecordings()) {
        setEnoughRecordings(enoughRecordings);
    }
}

void PatternWindow::on_playRecordingButton_clicked() {
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

void PatternWindow::on_PatternWindow_editModeChanged() {
    if (isEditMode) {
        this->setWindowTitle("Edit Pattern");
        this->ui->newPatternLabel->setText("Edit an existing Pattern");
        this->ui->createButton->setText("Save Pattern");
    } else {
        this->setWindowTitle("Create Pattern");
        this->ui->newPatternLabel->setText("Create a new Pattern");
        this->ui->createButton->setText("Create Pattern");
    }
}

void PatternWindow::on_PatternWindow_recordingChanged() {
    this->ui->toggleRecordingButton->setText(isRecording ? "Stop Recording" : "Start Recording");
}

void PatternWindow::on_recordingsListWidget_itemSelectionChanged() {
    bool isSelected = !this->ui->recordingsListWidget->selectedItems().empty();
    if (isSelected) {
        auto& selectedItem = this->ui->recordingsListWidget->selectedItems().first();
        auto idx = selectedItem->data(Qt::UserRole).toInt();
        setSelectedRecording(&_recordings[idx]);
    } else {
        setSelectedRecording(nullptr);
    }
    setRecordingSelected(isSelected);
}

void PatternWindow::checkPlayButton() {
    bool enabled = !isRecording && isRecordingSelected && !isPlayback;
    emit playButtonEnabledChanged(enabled);
}
void PatternWindow::checkDeleteButton() {
    bool enabled = !isRecording && isRecordingSelected && !isPlayback;
    emit deleteButtonEnabledChanged(enabled);
}
void PatternWindow::checkToggleButton() {
    bool enabled = !isPlayback;
    emit toggleButtonEnabledChanged(enabled);
}
void PatternWindow::checkCreateButton() {
    bool enabled = isNameValid && isEnoughRecordings && !isPlayback && !isRecording;
    emit createButtonEnabledChanged(enabled);
}

void PatternWindow::checkCancelButton() {
    bool enabled = !isPlayback && !isRecording;
    emit cancelButtonEnabledChanged(enabled);
}

