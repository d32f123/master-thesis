cmake_minimum_required(VERSION 3.5)

project(TwitFil VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL Concurrent Multimedia Charts REQUIRED)

find_library(ZMQ NAMES zmq)
find_library(ZMQPP NAMES zmqpp)
find_path(ZMQPP_INCLUDES NAMES zmqpp/zmqpp.hpp)
find_package(Python3 COMPONENTS Interpreter Development)

set(PROJECT_DIRS ui io patterns cython misc)

set(PROJECT_SOURCES
        main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/mainwindow.ui
        io/iodeviceplotter.cpp
        io/iodeviceplotter.h
        io/iodeviceplotterwidget.cpp
        io/iodeviceplotterwidget.h
        io/iodevicerecorder.cpp
        io/iodevicerecorder.h
        patterns/patternmodel.cpp
        patterns/patternmodel.h
        patterns/patternservice.cpp
        patterns/patternservice.h
        ui/patternwindow.cpp
        ui/patternwindow.h
        ui/patternwindow.ui
        cython/recognizer.cpp
        cython/recognizer.h
        misc/worker.h)

add_executable(TwitFilC ${PROJECT_SOURCES})

include_directories(${ZMQPP_INCLUDES} ${Python3_INCLUDE_DIRS})
target_link_options(TwitFilC PRIVATE ${Python3_LINK_OPTIONS})
target_link_directories(TwitFilC PRIVATE ${Python3_LIBRARY_DIRS})
target_link_libraries(TwitFilC PRIVATE Qt5::Widgets Qt5::Charts Qt5::Concurrent Qt5::Multimedia ${ZMQ} ${ZMQPP} ${Python3_LIBRARIES})
target_include_directories(TwitFilC PRIVATE ${PROJECT_DIRS})

set_target_properties(TwitFilC PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )
