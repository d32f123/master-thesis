QT       += core gui charts concurrent multimedia testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    driveractionsprovider.cpp \
    iodeviceplotter.cpp \
    iodeviceplotterwidget.cpp \
    iodevicerecorder.cpp \
    mainwindow.cpp \
    patternmodel.cpp \
    patternservice.cpp \
    patternwindow.cpp \
    testpatternwindow.cpp

HEADERS += \
    driveractionsprovider.h \
    iodeviceplotter.h \
    iodeviceplotterwidget.h \
    iodevicerecorder.h \
    mainwindow.h \
    patternmodel.h \
    patternservice.h \
    patternwindow.h

FORMS += \
    mainwindow.ui \
    patternwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
