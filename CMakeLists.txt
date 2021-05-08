cmake_minimum_required(VERSION 3.5)

project(TwitFil VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL Concurrent Multimedia Charts REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        driveractionsprovider.cpp
        driveractionsprovider.h
        iodeviceplotter.cpp
        iodeviceplotter.h
        iodeviceplotterwidget.cpp
        iodeviceplotterwidget.h
        iodevicerecorder.cpp
        iodevicerecorder.h
        patternmodel.cpp
        patternmodel.h
        patternservice.cpp
        patternservice.h
        patternwindow.cpp
        patternwindow.h
        patternwindow.ui
        )

add_executable(TwitFilC ${PROJECT_SOURCES})

target_link_libraries(TwitFilC PRIVATE Qt5::Widgets Qt5::Charts Qt5::Concurrent Qt5::Multimedia)

set_target_properties(TwitFilC PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )
