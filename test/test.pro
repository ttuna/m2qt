#-------------------------------------------------
#
# Project created by QtCreator 2016-05-07T00:55:02
#
#-------------------------------------------------

QT += core
QT += testlib
QT += network
QT += websockets
QT += concurrent
QT -= gui

TARGET = tst_m2qt
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

windows {
    contains(QT_ARCH, i386) {
        PLATFORM = Win32
    }else {
        PLATFORM = x64
    }

    win32-msvc2012: PLATFORM_TOOLSET = v110
    else:win32-msvc2013: PLATFORM_TOOLSET = v120

    CONFIG(debug, debug|release) {
        CONFIGURATION = Debug
    } else {
        CONFIGURATION = Release
    }

    LINKAGE = dynamic
}

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

INCLUDE_DIR = $$PWD/../include

SOURCES += tst_m2qt.cpp
HEADERS += $$INCLUDE_DIR/m2qt.h

windows {
#    SODIUM_DIR = c:/MyTools/ZeroMQ/libsodium
#    ZMQ_DIR = c:/MyTools/ZeroMQ/libzmq
#    CPP_ZMQ_DIR = c:/MyTools/ZeroMQ/cppzmq

    SODIUM_DIR = e:/MyTools/ZeroMQ/libsodium
    ZMQ_DIR = e:/MyTools/ZeroMQ/libzmq
    CPP_ZMQ_DIR = e:/MyTools/ZeroMQ/cppzmq
}

INCLUDEPATH += \
    $$INCLUDE_DIR \
    $$SODIUM_DIR/src/libsodium/include \
    $$ZMQ_DIR/include \
    $$CPP_ZMQ_DIR
DEPENDPATH += \
    $$INCLUDE_DIR \
    $$SODIUM_DIR/src/libsodium/include \
    $$ZMQ_DIR/include \
    $$CPP_ZMQ_DIR

windows {
    LIBS += -L$$SODIUM_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibsodium
    LIBS += -L$$ZMQ_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibzmq
}

include(../src/m2qt.pri)
