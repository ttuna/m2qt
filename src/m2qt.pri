INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += $$PWD/../build   #for .moc file ???
DEPENDPATH += $$PWD/../build    #for .moc file ???

SOURCES += $$PWD/serverconnection.cpp \
           $$PWD/m2qt.cpp \
    $$PWD/messageparser.cpp

HEADERS += $$PWD/serverconnection.h \
           $$PWD/m2qt_global.h \
    $$PWD/messageparser.h

