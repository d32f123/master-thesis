#include <algorithm>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "patternwindow.h"
#include "ui_patternwindow.h"
#include "cython/preprocessor.h"

#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        recognizer(std::make_unique<Recognizer>(this)) {
    ui->setupUi(this);

    connect(&*recognizer, &Recognizer::valueFoundChanged, this, &MainWindow::driverActionOccurred);
    connect(&*recognizer, &Recognizer::launchedChanged, this, &MainWindow::onRecognizingChanged);

    connect(this, &MainWindow::inactiveItemSelectedChanged, this, &MainWindow::checkActiveButton);
    connect(this, &MainWindow::activeItemSelectedChanged, this, &MainWindow::checkInactiveButton);
    connect(this, &MainWindow::itemSelectedChanged, this, &MainWindow::checkEditButton);

    connect(this, &MainWindow::recognizingChanged, this, &MainWindow::checkActiveButton);
    connect(this, &MainWindow::recognizingChanged, this, &MainWindow::checkInactiveButton);
    connect(this, &MainWindow::recognizingChanged, this, &MainWindow::checkEditButton);

    connect(this, &MainWindow::activeItemsAvailableChanged, this, &MainWindow::checkRecognizerButton);

    updatePatterns();
    falsePattern = PatternService::getFalsePattern();

    ui->micInputGraph->initialize(QAudioDeviceInfo::defaultInputDevice());
    ui->micInputGraph->start();
}

MainWindow::~MainWindow() {
    delete ui;
    if (recognizer->launched()) {
        recognizer->stop();
    }
}

bool MainWindow::itemSelected() const {
    return _itemSelected;
}

void MainWindow::setItemSelected(bool selected) {
    _itemSelected = selected;
    emit itemSelectedChanged();
}

bool MainWindow::activeItemSelected() const {
    return _activeItemSelected;
}

void MainWindow::setActiveItemSelected(bool v) {
    _activeItemSelected = v;
    emit activeItemSelectedChanged();

    if ((!v && !_inactiveItemSelected && _itemSelected) || (v && !_itemSelected)) {
        setItemSelected(v);
    };
}

bool MainWindow::inactiveItemSelected() const {
    return _inactiveItemSelected;
}

void MainWindow::setInactiveItemSelected(bool v) {
    _inactiveItemSelected = v;
    emit inactiveItemSelectedChanged();

    if ((!v && !_activeItemSelected && _itemSelected) || (v && !_itemSelected)) {
        setItemSelected(v);
    };
}

bool MainWindow::activeItemsAvailable() const {
    return _activeItemsAvailable;
}

void MainWindow::setActiveItemsAvailable(bool v) {
    _activeItemsAvailable = v;
    emit activeItemsAvailableChanged();
}

bool MainWindow::recognizing() const {
    return _recognizing;
}

void MainWindow::setRecognizing(bool v) {
    _recognizing = v;
    emit recognizingChanged();
}

void MainWindow::on_addPatternButton_clicked() {
    openPatternWindow(nullptr, [this](PatternModel *model) {
        this->patterns.push_back(*model);
        this->ui->inactivePatternsWidget->addItem(model->name());
        delete model;
    });
}

void MainWindow::updatePatterns() {
    patterns = PatternService::getPatterns();
    ui->inactivePatternsWidget->clear();
    ui->activePatternsWidget->clear();

    bool activeItemsPresent = false;
    for (auto &&pattern : patterns) {
        if (pattern.active()) {
            activeItemsPresent = true;
            ui->activePatternsWidget->addItem(pattern.name());
        } else {
            ui->inactivePatternsWidget->addItem(pattern.name());
        }
    }
    setActiveItemsAvailable(activeItemsPresent);
}

void MainWindow::on_activatePatternButton_clicked() {
    movePattern(ui->inactivePatternsWidget, ui->activePatternsWidget, true);
    setActiveItemsAvailable(true);
}

void MainWindow::on_deactivatePatternButton_clicked() {
    movePattern(ui->activePatternsWidget, ui->inactivePatternsWidget, false);
    if (ui->activePatternsWidget->count() == 0) setActiveItemsAvailable(false);
}

void MainWindow::movePattern(QListWidget *from, QListWidget *to, bool toActive) {
    auto name = from->selectedItems().first()->text();
    auto &pattern = *std::find_if(patterns.begin(), patterns.end(),
                                  [&name](const PatternModel &p) { return p.name() == name; });

    pattern.setActive(toActive);
    PatternService::savePatterns(patterns);

    delete from->selectedItems().first();
    to->addItem(pattern.name());
}

void MainWindow::openPatternWindow(PatternModel *model, const std::function<void(PatternModel *)> &acceptedCallback) {
    patternWindow = new PatternWindow(model, this);
    patternWindowConnections.clear();
    patternWindowConnections.push_back(
            connect(patternWindow, &PatternWindow::finished, this, [=](int result) {
                for (auto &&connection : patternWindowConnections) {
                    disconnect(connection);
                }

                if (result != QDialog::Accepted && !model) {
                    delete patternWindow->patternModel();
                }

                if (result == QDialog::Accepted) {
                    acceptedCallback(patternWindow->patternModel());
                }

                delete patternWindow;
            })
    );

    patternWindow->open();
}

QString MainWindow::getSelectedItem() const {
    if (!ui->activePatternsWidget->selectedItems().empty()) {
        return ui->activePatternsWidget->selectedItems().first()->text();
    } else {
        return ui->inactivePatternsWidget->selectedItems().first()->text();
    }
}

void MainWindow::on_deletePatternButton_clicked() {
    QString selectedName = getSelectedItem();
    if (!ui->activePatternsWidget->selectedItems().empty()) {
        delete ui->activePatternsWidget->selectedItems().first();
        if (ui->activePatternsWidget->count() == 0) setActiveItemsAvailable(false);
    } else {
        delete ui->inactivePatternsWidget->selectedItems().first();
    }

    auto removedModelIt = std::find_if(
            patterns.begin(),
            patterns.end(),
            [&selectedName](const PatternModel &p) { return p.name() == selectedName; }
    );
    PatternService::deletePattern(*removedModelIt);
    patterns.erase(removedModelIt);
    if (ui->activePatternsWidget->count() == 0) setActiveItemsAvailable(false);
}

void MainWindow::on_inactivePatternsWidget_itemSelectionChanged() {
    bool isSelected = !ui->inactivePatternsWidget->selectedItems().empty();
    setInactiveItemSelected(isSelected);
}

void MainWindow::on_activePatternsWidget_itemSelectionChanged() {
    bool isSelected = !ui->activePatternsWidget->selectedItems().empty();
    setActiveItemSelected(isSelected);
}

void MainWindow::driverActionOccurred(bool actionType) {
    if (actionType) {
        ui->micStatusWidget->setStyleSheet("background-color: green;");
    } else {
        ui->micStatusWidget->setStyleSheet("background-color: red;");
    }
}

void MainWindow::on_editPatternButton_clicked() {
    QString selectedName = getSelectedItem();
    openPatternWindow(std::find_if(patterns.begin(), patterns.end(), [=](const PatternModel &p) {
        return p.name() == selectedName;
    }), [this](PatternModel *model) {
        if (!ui->activePatternsWidget->selectedItems().empty()) {
            ui->activePatternsWidget->selectedItems().first()->setText(model->name());
        } else {
            ui->inactivePatternsWidget->selectedItems().first()->setText(model->name());
        }
    });
}

void MainWindow::on_falsePatternButton_clicked() {
    openPatternWindow(&falsePattern, [](PatternModel *model) {});
}

void MainWindow::on_toggleRecognizerButton_clicked() {
    if (falsePattern.recordings().size() < 3) {
        on_falsePatternButton_clicked();
        return;
    }

    if (recognizer->launched()) {
        recognizer->stop();
        ui->toggleRecognizerButton->setEnabled(false);
        setRecognizing(false);
    } else {
        recognizer->launch(patterns);
        setRecognizing(true);
    }
}

void MainWindow::checkInactiveButton() {
    emit toInactiveButtonEnabledChanged(_activeItemSelected && !_recognizing);
}

void MainWindow::checkActiveButton() {
    emit toActiveButtonEnabledChanged(_inactiveItemSelected && !_recognizing);
}

void MainWindow::checkEditButton() {
    emit editButtonEnabledChanged(_itemSelected && !_recognizing);
}

void MainWindow::checkRecognizerButton() {
    emit recognizerButtonEnabledChanged(_activeItemsAvailable);
}

void MainWindow::onRecognizingChanged(bool v) {
    if (!v) {
        ui->toggleRecognizerButton->setText("Launch Recognizer");
        ui->toggleRecognizerButton->setEnabled(true);
        ui->micStatusWidget->setStyleSheet("background-color: gray;");
    } else {
        ui->toggleRecognizerButton->setText("Stop Recognizer");
        ui->micStatusWidget->setStyleSheet("background-color: red;");
    }
}
