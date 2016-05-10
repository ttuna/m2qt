QT += core
QT -= gui

CONFIG += c++11

TARGET = M2ppApp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
windows {
    PLATFORM = Win32
    PLATFORM_TOOLSET = v120
    LINKAGE = dynamic
    CONFIG(debug, debug|release) {
        CONFIGURATION = Debug
    } else {
        CONFIGURATION = Release
    }
}

DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_RESTRICTED_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

INCLUDE_DIR = $$PWD/../include

windows {
    SODIUM_DIR = c:/MyTools/ZeroMQ/libsodium
    ZMQ_DIR = c:/MyTools/ZeroMQ/libzmq
    CPP_ZMQ_DIR = c:/MyTools/ZeroMQ/cppzmq
    M2PP_DIR = C:\MyTools\mongrel2-cpp\lib
}

SOURCES += main.cpp

INCLUDEPATH += \
    $$INCLUDE_DIR \
    $$SODIUM_DIR/src/libsodium/include \
    $$ZMQ_DIR/include \
    $$CPP_ZMQ_DIR \
    $$M2PP_DIR
DEPENDPATH += \
    $$INCLUDE_DIR \
    $$SODIUM_DIR/src/libsodium/include \
    $$ZMQ_DIR/include \
    $$CPP_ZMQ_DIR \
    $$M2PP_DIR

windows {
    LIBS += -L$$SODIUM_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibsodium
    LIBS += -L$$ZMQ_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibzmq
    LIBS += -L$$M2PP_DIR -llibm2pp
}
