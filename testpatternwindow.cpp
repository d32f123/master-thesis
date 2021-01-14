#include "patternwindow.h"
#include "patternmodel.h"

#include <QObject>
#include <QtTest/QtTest>

class TestPatternWindow : public QObject
{
    Q_OBJECT
public:
    explicit TestPatternWindow(QObject *parent = nullptr);

private slots:
    void testEditMode();
    void testCreateMode();
};

TestPatternWindow::TestPatternWindow(QObject *parent) : QObject(parent)
{}

void TestPatternWindow::testEditMode()
{
    PatternModel model;
    PatternWindow patternWindow(&model);

    QVERIFY(patternWindow.editMode());
}

void TestPatternWindow::testCreateMode()
{
    PatternWindow patternWindow;

    QVERIFY(!patternWindow.editMode());
}

QTEST_MAIN(TestPatternWindow)
#include "testpatternwindow.moc"
