#-------------------------------------------------
#
# Project created by QtCreator 2016-05-06T23:18:30
#
#-------------------------------------------------

QT -= gui
QT += core
QT += network
QT += websockets
QT += concurrent

TARGET = m2qt
TEMPLATE = lib

CONFIG += c++11

DEFINES += homeoffice
DEFINES += ENABLE_DEV_CALLBACKS
DEFINES += M2QTSHARED_EXPORT
DEFINES *= QT_USE_QSTRINGBUILDER
DEFINES *= QT_NO_CAST_TO_ASCII \
           QT_RESTRICTED_CAST_FROM_ASCII

INCLUDE_DIR = $$PWD/../include
INCLUDEPATH += $$INCLUDE_DIR
DEPENDPATH += $$INCLUDE_DIR

# build settings ...
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

# dependencies & destinations ...
windows {
    contains(DEFINES, homeoffice) {
        SODIUM_DIR = c:/MyTools/ZeroMQ/libsodium
        ZMQ_DIR = c:/MyTools/ZeroMQ/libzmq
        CPP_ZMQ_DIR = c:/MyTools/ZeroMQ/cppzmq
    } else {
        SODIUM_DIR = e:/MyTools/ZeroMQ/libsodium
        ZMQ_DIR = e:/MyTools/ZeroMQ/libzmq
        CPP_ZMQ_DIR = e:/MyTools/ZeroMQ/cppzmq
    }
    INCLUDEPATH += \
        $$SODIUM_DIR/src/libsodium/include \
        $$ZMQ_DIR/include \
        $$CPP_ZMQ_DIR
    DEPENDPATH += \
        $$SODIUM_DIR/src/libsodium/include \
        $$ZMQ_DIR/include \
        $$CPP_ZMQ_DIR

    DESTDIR = $$PWD/../bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET     #copy .lib file
    DLLDESTDIR = $$PWD/../bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET  #copy .dll file
}
unix {
    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include
}

# external libraries ...
windows {
    LIBS += -L$$SODIUM_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibsodium
    LIBS += -L$$ZMQ_DIR/bin/$$PLATFORM/$$CONFIGURATION/$$PLATFORM_TOOLSET/$$LINKAGE -llibzmq
}
unix {
    LIBS += -L/usr/local/lib -lsodium
    LIBS += -L/usr/local/lib -lzmq
}


# files ...
HEADERS += $$INCLUDE_DIR/m2qt.h

include(m2qt.pri)
