#include <algorithm>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "patternwindow.h"
#include "ui_patternwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), driverActionsProvider(new DriverActionsProvider(this)) {
    ui->setupUi(this);
    updatePatterns();
    connect(driverActionsProvider, &DriverActionsProvider::actionOccurred, this, &MainWindow::driverActionOccurred);
    ui->micInputGraph->initialize(QAudioDeviceInfo::defaultInputDevice());
    ui->micInputGraph->start();
}

MainWindow::~MainWindow() {
    delete ui;
    delete driverActionsProvider;
}

bool MainWindow::itemSelected() const {
    return _itemSelected;
}

void MainWindow::setItemSelected(bool selected) {
    _itemSelected = selected;
    emit itemSelectedChanged(selected);
}

void MainWindow::on_addPatternButton_clicked() {
    openPatternWindow(nullptr);
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

void MainWindow::openPatternWindow(PatternModel *model) {
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
                    if (!model) {
                        addPattern(patternWindow->patternModel());
                    } else {
                        patternService.savePatterns(patterns);
                        updatePatterns();
                    }
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
    emit inactiveItemSelectedChanged(isSelected);

    inactiveItemSelected = isSelected;
    if ((!isSelected && !activeItemSelected && _itemSelected) || (isSelected && !_itemSelected)) {
        setItemSelected(isSelected);
    };
}

void MainWindow::on_activePatternsWidget_itemSelectionChanged() {
    bool isSelected = !ui->activePatternsWidget->selectedItems().empty();
    emit activeItemSelectedChanged(isSelected);

    activeItemSelected = isSelected;
    if ((!isSelected && !inactiveItemSelected && _itemSelected) || (isSelected && !_itemSelected)) {
        setItemSelected(isSelected);
    };
}

void MainWindow::driverActionOccurred(bool actionType) {
    qInfo("Received action type %d", actionType);
}

void MainWindow::on_editPatternButton_clicked() {
    QString selectedName = getSelectedItem();
    openPatternWindow(std::find_if(patterns.begin(), patterns.end(), [=](const PatternModel &p) {
        return p.name() == selectedName;
    }));
}
