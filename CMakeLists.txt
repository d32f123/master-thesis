cmake_minimum_required(VERSION 3.5)

project(TwitFil VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL Concurrent Multimedia Charts REQUIRED)

set( ZMQ_NAME  "zmq" CACHE STRING "The ZMQ library to" )
set( ZMQPP_NAME "zmqpp" CACHE STRING "The ZMQPP Wrapper to link to" )

find_path(ZMQ_INCLUDES NAMES zmq.h REQUIRED)
find_path(ZMQPP_INCLUDES NAMES zmqpp/zmqpp.hpp REQUIRED)
file(READ "${ZMQ_INCLUDES}/zmq.h" _ZMQ_H_CONTENTS)
string(REGEX REPLACE ".*#define ZMQ_VERSION_MAJOR ([0-9]+).*" "\\1" ZMQ_VERSION_MAJOR "${_ZMQ_H_CONTENTS}")
string(REGEX REPLACE ".*#define ZMQ_VERSION_MINOR ([0-9]+).*" "\\1" ZMQ_VERSION_MINOR "${_ZMQ_H_CONTENTS}")
string(REGEX REPLACE ".*#define ZMQ_VERSION_PATCH ([0-9]+).*" "\\1" ZMQ_VERSION_PATCH "${_ZMQ_H_CONTENTS}")
set(ZMQ_VERSION "${ZMQ_VERSION_MAJOR}_${ZMQ_VERSION_MINOR}_${ZMQ_VERSION_PATCH}")
message(STATUS "Detected ZMQ Version - ${ZMQ_VERSION}")
list(APPEND ZMQ_NAME "libzmq-mt-${ZMQ_VERSION}")
message(STATUS "ZMQ libs: ${ZMQ_NAME}")

list(APPEND ZMQPP_NAME "libzmqpp")

find_library(ZMQ NAMES ${ZMQ_NAME} REQUIRED)
find_library(ZMQPP NAMES ${ZMQPP_NAME} REQUIRED)

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
        misc/worker.h
        cython/preprocessor.cpp
        cython/preprocessor.h
        config/config.cpp
        config/config.h)

add_executable(TwitFilC ${PROJECT_SOURCES})

include_directories(${ZMQ_INCLUDES} ${ZMQPP_INCLUDES} ${Python3_INCLUDE_DIRS})
target_link_options(TwitFilC PRIVATE ${Python3_LINK_OPTIONS})
target_link_directories(TwitFilC PRIVATE ${Python3_LIBRARY_DIRS})
target_link_libraries(TwitFilC PRIVATE Qt5::Widgets Qt5::Charts Qt5::Concurrent Qt5::Multimedia ${ZMQ} ${ZMQPP} ${Python3_LIBRARIES})
target_include_directories(TwitFilC PRIVATE ${PROJECT_DIRS})

set_target_properties(TwitFilC PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )
