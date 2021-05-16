#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>

#include <QMainWindow>
#include <QStringList>
#include <QListWidget>

#include "patterns/patternservice.h"
#include "patternwindow.h"
#include "cython/recognizer.h"
#include "cython/preprocessor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    [[nodiscard]] bool itemSelected() const;
    void setItemSelected(bool v);

    [[nodiscard]] bool activeItemSelected() const;
    void setActiveItemSelected(bool v);

    [[nodiscard]] bool inactiveItemSelected() const;
    void setInactiveItemSelected(bool v);

    [[nodiscard]] bool activeItemsAvailable() const;
    void setActiveItemsAvailable(bool v);

    [[nodiscard]] bool recognizing() const;
    void setRecognizing(bool v);

signals:
    void itemSelectedChanged();
    void inactiveItemSelectedChanged();
    void activeItemSelectedChanged();
    void activeItemsAvailableChanged();
    void recognizingChanged();

    void toInactiveButtonEnabledChanged(bool);
    void toActiveButtonEnabledChanged(bool);
    void editButtonEnabledChanged(bool);
    void recognizerButtonEnabledChanged(bool);

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
    void checkRecognizerButton();
    void onRecognizingChanged(bool v);

    void updatePatterns();

private:
    Ui::MainWindow *ui;
    PatternWindow *patternWindow {nullptr};
    QVector<QMetaObject::Connection> patternWindowConnections;

    std::unique_ptr<Recognizer> recognizer;

    QVector<PatternModel> patterns;
    PatternModel falsePattern {};

    void movePattern(QListWidget *from, QListWidget *to, bool toActive);
    void openPatternWindow(PatternModel *model, const std::function<void(PatternModel*)>& acceptedCallback);

    [[nodiscard]] QString getSelectedItem() const;

    bool _inactiveItemSelected = false;
    bool _activeItemSelected = false;
    bool _itemSelected = false;
    bool _activeItemsAvailable = false;
    bool _recognizing = false;
};

#endif // MAINWINDOW_H
