#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <functional>

#include <QMainWindow>
#include <QStringList>
#include <QListWidget>

#include "patternservice.h"
#include "patternwindow.h"
#include "recognizer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT;

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    bool itemSelected() const;
    void setItemSelected(bool v);

    bool activeItemSelected() const;
    void setActiveItemSelected(bool v);

    bool inactiveItemSelected() const;
    void setInactiveItemSelected(bool v);

signals:
    void itemSelectedChanged();
    void inactiveItemSelectedChanged();
    void activeItemSelectedChanged();

    void toInactiveButtonEnabledChanged(bool);
    void toActiveButtonEnabledChanged(bool);
    void editButtonEnabledChanged(bool);

public slots:
    void driverActionOccurred(bool actionType);

private slots:
    void on_addPatternButton_clicked();
    void on_activatePatternButton_clicked();
    void on_deactivatePatternButton_clicked();
    void on_deletePatternButton_clicked();
    void on_inactivePatternsWidget_itemSelectionChanged();
    void on_activePatternsWidget_itemSelectionChanged();
    void on_editPatternButton_clicked();
    void on_falsePatternButton_clicked();
    void on_toggleRecognizerButton_clicked();

    void checkInactiveButton();
    void checkActiveButton();
    void checkEditButton();
    void recognizerLaunchedChanged(bool v);

    void updatePatterns();
    void addPattern(PatternModel *model);

private:
    Ui::MainWindow *ui;
    PatternWindow *patternWindow;
    QVector<QMetaObject::Connection> patternWindowConnections;

    Recognizer *recognizer;
    PatternService patternService {};

    QVector<PatternModel> patterns;
    PatternModel falsePattern {};

    void movePattern(QListWidget *from, QListWidget *to, bool toActive);
    void openPatternWindow(PatternModel *model, const std::function<void(PatternModel*)>& acceptedCallback);

    QString getSelectedItem() const;

    bool _inactiveItemSelected = false;
    bool _activeItemSelected = false;
    bool _itemSelected = false;
};

#endif // MAINWINDOW_H
