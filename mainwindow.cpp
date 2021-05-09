#include <algorithm>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "patternwindow.h"
#include "ui_patternwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), recognizer(new Recognizer(this)) {
    ui->setupUi(this);

    updatePatterns();
    falsePattern = patternService.getFalsePattern();

    connect(recognizer, &Recognizer::valueFoundChanged, this, &MainWindow::driverActionOccurred);
    connect(recognizer, &Recognizer::launchedChanged, this, &MainWindow::on_recognizer_launchedChanged);

    connect(this, &MainWindow::inactiveItemSelectedChanged, this, &MainWindow::checkActiveButton);
    connect(this, &MainWindow::activeItemSelectedChanged, this, &MainWindow::checkInactiveButton);
    connect(this, &MainWindow::itemSelectedChanged, this, &MainWindow::checkEditButton);

    ui->micInputGraph->initialize(QAudioDeviceInfo::defaultInputDevice());
    ui->micInputGraph->start();
}

MainWindow::~MainWindow() {
    delete ui;
    if (recognizer->launched()) {
        recognizer->stop();
    }
    delete recognizer;
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

void MainWindow::on_addPatternButton_clicked() {
    openPatternWindow(nullptr, [this](PatternModel *model) {
        this->addPattern(model);
    });
}

void MainWindow::updatePatterns() {
    patterns = patternService.getPatterns();
    ui->inactivePatternsWidget->clear();
    ui->activePatternsWidget->clear();

    for (auto &&pattern : patterns) {
        if (pattern.active()) {
            ui->activePatternsWidget->addItem(pattern.name());
        } else {
            ui->inactivePatternsWidget->addItem(pattern.name());
        }
    }
}

void MainWindow::addPattern(PatternModel *model) {
    patternService.addPattern(*model);
    delete model;
    updatePatterns();
}

void MainWindow::on_activatePatternButton_clicked() {
    movePattern(ui->inactivePatternsWidget, ui->activePatternsWidget, true);
}

void MainWindow::on_deactivatePatternButton_clicked() {
    movePattern(ui->activePatternsWidget, ui->inactivePatternsWidget, false);
}

void MainWindow::movePattern(QListWidget *from, QListWidget *to, bool toActive) {
    auto name = from->selectedItems().first()->text();
    auto &pattern = *std::find_if(patterns.begin(), patterns.end(),
                                  [&name](const PatternModel &p) { return p.name() == name; });

    pattern.setActive(toActive);
    patternService.savePatterns(patterns);

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
    } else {
        delete ui->inactivePatternsWidget->selectedItems().first();
    }

    patterns.erase(
            std::remove_if(
                    patterns.begin(),
                    patterns.end(),
                    [&selectedName](const PatternModel &p) { return p.name() == selectedName; }),
            patterns.end()
    );
    patternService.savePatterns(patterns);
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
    qInfo("Received action type %d", actionType);
}

void MainWindow::on_editPatternButton_clicked() {
    QString selectedName = getSelectedItem();
    openPatternWindow(std::find_if(patterns.begin(), patterns.end(), [=](const PatternModel &p) {
        return p.name() == selectedName;
    }), [this](PatternModel *model) {
        patternService.savePatterns(patterns);
        updatePatterns();
    });
}

void MainWindow::on_falsePatternButton_clicked() {
    openPatternWindow(&falsePattern, [this](PatternModel *model) {
        patternService.saveFalsePattern(falsePattern);
    });
}

void MainWindow::on_toggleRecognizerButton_clicked() {
    if (falsePattern.recordings().size() < 3) {
        on_falsePatternButton_clicked();
        return;
    }

    if (recognizer->launched()) {
        recognizer->stop();
    } else {
        recognizer->launch(patterns);
    }
}

void MainWindow::checkInactiveButton() {
    emit toInactiveButtonEnabledChanged(_activeItemSelected);
}

void MainWindow::checkActiveButton() {
    emit toActiveButtonEnabledChanged(_inactiveItemSelected);
}

void MainWindow::checkEditButton() {
    emit editButtonEnabledChanged(_itemSelected);
}

void MainWindow::on_recognizer_launchedChanged(bool v) {
    ui->toggleRecognizerButton->setText(v ? "Stop Recognizer" : "Launch Recognizer");
}

