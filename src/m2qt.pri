INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#INCLUDEPATH += $$PWD/../build   #for .moc file ???
#DEPENDPATH += $$PWD/../build    #for .moc file ???

SOURCES += \
    $$PWD/m2qt.cpp \
    $$PWD/m2qt_messageparser.cpp \
    $$PWD/m2qt_serverconnection.cpp \
    $$PWD/m2qt_handler.cpp

HEADERS += \
    $$PWD/m2qt_global.h \
    $$PWD/m2qt_messageparser.h \
    $$PWD/m2qt_serverconnection.h \
    $$PWD/m2qt_handler.h
