
QT -= gui
QT += core
QT += network
QT += websockets

CONFIG += c++11
CONFIG += homeoffice

message($$CONFIG)

TARGET = M2QtTest
CONFIG += console
CONFIG -= app_bundle

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

DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

INCLUDE_DIR = $$PWD/../include

DESTDIR = $$PWD/../bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET     #copy .lib file
DLLDESTDIR = $$PWD/../bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET  #copy .dll file

windows {
    CONFIG(homeoffice) {
        SODIUM_DIR = c:/MyTools/ZeroMQ/libsodium
        ZMQ_DIR = c:/MyTools/ZeroMQ/libzmq
        CPP_ZMQ_DIR = c:/MyTools/ZeroMQ/cppzmq
    } else {
        SODIUM_DIR = e:/MyTools/ZeroMQ/libsodium
        ZMQ_DIR = e:/MyTools/ZeroMQ/libzmq
        CPP_ZMQ_DIR = e:/MyTools/ZeroMQ/cppzmq
    }
    message($$SODIUM_DIR $$ZMQ_DIR $$CPP_ZMQ_DIR)
}

SOURCES += main.cpp \
    websockethelper.cpp

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
    message($$LIBS)
}

HEADERS += \
    websockethelper.h
