#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QListWidget>

#include "patternservice.h"
#include "patternwindow.h"
#include "driveractionsprovider.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT;

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    bool itemSelected() const;

    void setItemSelected(bool selected);

signals:

    void itemSelectedChanged(bool active);

    void inactiveItemSelectedChanged(bool active);

    void activeItemSelectedChanged(bool active);

public slots:

    void driverActionOccurred(bool actionType);

private slots:

    void on_addPatternButton_clicked();

    void updatePatterns();

    void addPattern(PatternModel *model);

    void on_activatePatternButton_clicked();

    void on_deactivatePatternButton_clicked();

    void on_deletePatternButton_clicked();

    void on_inactivePatternsWidget_itemSelectionChanged();

    void on_activePatternsWidget_itemSelectionChanged();

    void on_editPatternButton_clicked();

private:
    Ui::MainWindow *ui;
    DriverActionsProvider *driverActionsProvider;
    PatternService patternService;
    QVector<PatternModel> patterns;

    void movePattern(QListWidget *from, QListWidget *to, bool toActive);

    void openPatternWindow(PatternModel *model);

    QString getSelectedItem() const;

    bool inactiveItemSelected;
    bool activeItemSelected;
    bool _itemSelected;

    PatternWindow *patternWindow;
    QVector<QMetaObject::Connection> patternWindowConnections;
};

#endif // MAINWINDOW_H
