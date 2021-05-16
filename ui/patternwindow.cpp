#include "patternwindow.h"
#include "ui_patternwindow.h"

#include <QtMultimedia/QAudioOutput>

#include "io/iodevicerecorder.h"
#include "patterns/patternservice.h"

PatternWindow::PatternWindow(PatternModel *model, QWidget *parent) :
        QDialog(parent, Qt::Dialog),
        ui(new Ui::PatternWindow),
        recorder(std::make_unique<IODeviceRecorder>(QAudioDeviceInfo::defaultInputDevice(), this)),
        preprocessor(std::make_unique<Preprocessor>(this)),
        model(model) {
    ui->setupUi(this);
    ui->ioPlotter->initialize(QAudioDeviceInfo::defaultInputDevice());

    connect(ui->createButton, &QPushButton::clicked, this, &PatternWindow::onCreateButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &PatternWindow::onCancelButtonClicked);
    connect(ui->toggleRecordingButton, &QPushButton::clicked, this, &PatternWindow::onToggleRecordingButtonClicked);
    connect(ui->playRecordingButton, &QPushButton::clicked, this, &PatternWindow::onPlayRecordingButtonClicked);
    connect(ui->deleteRecordingButton, &QPushButton::clicked, this, &PatternWindow::onDeleteRecordingButtonClicked);
    connect(ui->patternNameField, &QLineEdit::textChanged, this, &PatternWindow::onPatternNameFieldTextChanged);
    connect(ui->recordingsListWidget, &QListWidget::itemSelectionChanged, this, &PatternWindow::onRecordingsListWidgetItemSelectionChanged);

    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkDeleteButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkCreateButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::checkCancelButton);
    connect(this, &PatternWindow::recordingChanged, this, &PatternWindow::onRecordingChanged);

    connect(this, &PatternWindow::editModeChanged, this, &PatternWindow::onEditModeChanged);

    connect(this, &PatternWindow::recordingSelectedChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::recordingSelectedChanged, this, &PatternWindow::checkDeleteButton);

    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkDeleteButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkToggleButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkCreateButton);
    connect(this, &PatternWindow::playbackChanged, this, &PatternWindow::checkCancelButton);

    connect(this, &PatternWindow::nameValidChanged, this, &PatternWindow::checkCreateButton);

    connect(this, &PatternWindow::enoughRecordingsChanged, this, &PatternWindow::checkCreateButton);

    connect(this, &PatternWindow::dirtyChanged, this, &PatternWindow::checkCancelButton);
    connect(this, &PatternWindow::dirtyChanged, this, &PatternWindow::onDirtyChanged);

    connect(this, &PatternWindow::renamedChanged, this, &PatternWindow::onDirtyChanged);

    connect(this, &PatternWindow::savingChanged, this, &PatternWindow::checkCancelButton);
    connect(this, &PatternWindow::savingChanged, this, &PatternWindow::checkCreateButton);
    connect(this, &PatternWindow::savingChanged, this, &PatternWindow::checkDeleteButton);
    connect(this, &PatternWindow::savingChanged, this, &PatternWindow::checkPlayButton);
    connect(this, &PatternWindow::savingChanged, this, &PatternWindow::checkToggleButton);

    connect(this, &PatternWindow::savedChanged, this, &PatternWindow::checkCancelButton);

    connect(&*preprocessor, &Preprocessor::finished, this, &PatternWindow::onPreprocessingDone, Qt::QueuedConnection);

    setEditMode(model);
    if (!model) {
        this->model = model = new PatternModel();
    }
    ui->patternNameField->setText(model->name());

    QListWidgetItem* first = nullptr;
    for (auto&& recording : model->recordings()) {
        auto item = add_recording(recording);
        if (first == nullptr) first = item;
    }
    if (first) ui->recordingsListWidget->setCurrentItem(first);

    checkNameValid(model->name());
    checkEnoughRecordings();
    setDirty(false);
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

bool PatternWindow::dirty() const {
    return isDirty;
}

bool PatternWindow::renamed() const {
    return isRenamed;
}

bool PatternWindow::saving() const {
    return isSaving;
}

bool PatternWindow::saved() const {
    return isSaved;
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

void PatternWindow::setDirty(bool v) {
    isDirty = v;
    emit dirtyChanged();
}

void PatternWindow::setRenamed(bool v) {
    isRenamed = v;
    emit renamedChanged();
}

void PatternWindow::setSaving(bool v) {
    isSaving = v;
    emit savingChanged();
}

void PatternWindow::setSaved(bool v) {
    isSaved = v;
    emit savedChanged();
}

void PatternWindow::setSelectedRecording(PatternRecording *v) {
    _selectedRecording = v;
    ui->ioPlotter->setAudio(v ? v->bytes : QByteArray());
    recorder->set(v ? v->bytes : QByteArray());
}

void PatternWindow::onCreateButtonClicked() {
    if (!dirty() && !renamed()) {
        accept();
    } else {
        save();
    }
}

void PatternWindow::onCancelButtonClicked() {
    reject();
}

void PatternWindow::onPatternNameFieldTextChanged(const QString &arg1) {
    checkNameValid(arg1);
    setRenamed(true);
}

QListWidgetItem* PatternWindow::add_recording(const PatternRecording &recording) {
    _recordings.push_back(recording);
    int idx = _recordings.size() - 1;

    QString name = "Recording ";
    name.append(std::to_string(idx + 1).c_str());
    auto* item = new QListWidgetItem{name};
    item->setData(Qt::UserRole, idx);

    ui->recordingsListWidget->addItem(item);
    return item;
}

void PatternWindow::onToggleRecordingButtonClicked() {
    if (!recording()) {
        recorder->set(QByteArray());
        recorder->start();
        ui->ioPlotter->start();
        setRecording(true);
    } else {
        recorder->stop();
        ui->ioPlotter->stop();
        setRecording(false);

        auto item = add_recording({.bytes = recorder->buf->buffer()});
        ui->recordingsListWidget->setCurrentItem(item);

        checkEnoughRecordings();
        setDirty(true);
    }
}

void PatternWindow::checkEnoughRecordings() {
    bool enoughRecordings = _recordings.size() >= PatternWindow::MIN_RECORDINGS_COUNT;
    if (enoughRecordings != this->enoughRecordings()) {
        setEnoughRecordings(enoughRecordings);
    }
}

void PatternWindow::checkNameValid(const QString& s) {
    setNameValid(!s.isEmpty());
}

void PatternWindow::onDeleteRecordingButtonClicked() {
    auto item = ui->recordingsListWidget->selectedItems().first();
    auto idx = item->data(Qt::UserRole).toInt();
    for (auto i = idx + 1; i < ui->recordingsListWidget->count(); ++i) {
        ui->recordingsListWidget->item(i)->setData(Qt::UserRole, i - 1);
        ui->recordingsListWidget->item(i)->setText(QString(std::string("Recording " + std::to_string(i)).c_str()));
    }
    _recordings.removeAt(idx);
    delete item;

    checkEnoughRecordings();
    setDirty(true);
}

void PatternWindow::onPlayRecordingButtonClicked() {
    QAudioDeviceInfo audioInfo = QAudioDeviceInfo::defaultOutputDevice();
    QAudioFormat format = IODeviceRecorder::defaultFormat();
    if (!audioInfo.isFormatSupported(format)) {
        qWarning("Default format is unsupported for playback");
        return;
    }

    auto *audioOutput = new QAudioOutput(format, this);
    connect(audioOutput, &QAudioOutput::stateChanged, this, [this, audioOutput](QAudio::State state) -> void {
        switch (state) {
            case QAudio::IdleState:
                audioOutput->stop();
                setPlayback(false);
                break;
            case QAudio::StoppedState:
                if (audioOutput->error() != QAudio::NoError) {
                    qWarning("Error during playback: %d", audioOutput->error());
                }
                setPlayback(false);
                audioOutput->deleteLater();
                break;
            default:
                break;
        }
    });
    recorder->resetRead();
    audioOutput->start(recorder->buf);
    setPlayback(true);
}

void PatternWindow::onEditModeChanged() {
    this->setWindowTitle(getTitle());
    this->ui->newPatternLabel->setText(getLabelText());
    this->ui->createButton->setText(getCreateButtonText());
}

void PatternWindow::onRecordingChanged() {
    this->ui->toggleRecordingButton->setText(recording() ? "Stop Recording" : "Start Recording");
}

void PatternWindow::onRecordingsListWidgetItemSelectionChanged() {
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

QString PatternWindow::getTitle() const {
    return editMode() ? "Edit Pattern" : "Create Pattern";
}

QString PatternWindow::getCreateButtonText() const {
    return !dirty() && !renamed()
         ? "Close"
         : editMode()
         ? "Save Pattern"
         : "Create Pattern";
}

QString PatternWindow::getLabelText() const {
    return editMode() ? "Edit an existing Pattern" : "Create a new Pattern";
}

void PatternWindow::onDirtyChanged() {
    this->ui->createButton->setText(getCreateButtonText());
}

void PatternWindow::onPreprocessingDone() {
    PatternService::updatePattern(*model);
    _recordings = model->recordings();
    setDirty(false);
    setRenamed(false);
    setSaving(false);
    setSaved(true);
}

void PatternWindow::save() {
    if (!dirty() && !renamed()) return;

    auto new_name = ui->patternNameField->text();
    if (renamed() && editMode()) {
        PatternService::renamePattern(*model, new_name);
        setRenamed(false);
    }
    if (!dirty()) {
        setSaved(true);
        return;
    }

    setSaving(true);
    model->setRecordings(std::move(_recordings));
    PatternService::savePattern(*model);
    preprocessor->patternName = new_name;
    preprocessor->start();
}

void PatternWindow::checkPlayButton() {
    bool enabled = !recording() && recordingSelected() && !playback() && !saving();
    emit playButtonEnabledChanged(enabled);
}

void PatternWindow::checkDeleteButton() {
    bool enabled = !recording() && recordingSelected() && !playback() && !saving();
    emit deleteButtonEnabledChanged(enabled);
}

void PatternWindow::checkToggleButton() {
    bool enabled = !playback() && !saving();
    emit toggleButtonEnabledChanged(enabled);
}

void PatternWindow::checkCreateButton() {
    bool enabled = nameValid() && enoughRecordings() && !playback() && !recording() && !saving();
    emit createButtonEnabledChanged(enabled);
}

void PatternWindow::checkCancelButton() {
    bool enabled = !playback() && !recording() && !saved() && !saving();
    emit cancelButtonEnabledChanged(enabled);
}
