cmake_minimum_required(VERSION 3.5)
project(ZMQPP_Builder)

if (NOT MSVC)
    set(ZMQ_SHARED_NAME "zmq")
    set(ZMQ_STATIC_NAME "zmq-static")
else()
    find_path(ZMQ_INCLUDES NAMES zmq.h REQUIRED)
    file(READ "${ZMQ_INCLUDES}/zmq.h" _ZMQ_H_CONTENTS)
    string(REGEX REPLACE ".*#define ZMQ_VERSION_MAJOR ([0-9]+).*" "\\1" ZMQ_VERSION_MAJOR "${_ZMQ_H_CONTENTS}")
    string(REGEX REPLACE ".*#define ZMQ_VERSION_MINOR ([0-9]+).*" "\\1" ZMQ_VERSION_MINOR "${_ZMQ_H_CONTENTS}")
    string(REGEX REPLACE ".*#define ZMQ_VERSION_PATCH ([0-9]+).*" "\\1" ZMQ_VERSION_PATCH "${_ZMQ_H_CONTENTS}")
    set(ZMQ_VERSION "${ZMQ_VERSION_MAJOR}_${ZMQ_VERSION_MINOR}_${ZMQ_VERSION_PATCH}")
    message(STATUS "Detected ZMQ Version - ${ZMQ_VERSION}")

    set(ZMQ_SHARED_NAME "libzmq-mt-${ZMQ_VERSION}")
    set(ZMQ_STATIC_NAME "libzmq-s-mt-${ZMQ_VERSION}")
endif()
message(STATUS "ZMQ libs: ${ZMQ_SHARED_NAME}")

include(ExternalProject)

ExternalProject_Add(zmqpp SOURCE_DIR ${ZMQPP_LOC}/src BINARY_DIR ${ZMQPP_LOC}/build INSTALL_DIR ${ZMQPP_LOC}/install
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}" "-DZMQPP_BUILD_STATIC=OFF"
        "-DCMAKE_INSTALL_PREFIX=${ZMQPP_LOC}/install"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}" "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DZMQPP_LIBZMQ_NAME_STATIC=${ZMQ_STATIC_NAME}" "-DZMQPP_LIBZMQ_NAME_SHARED=${ZMQ_SHARED_NAME}")