QT += sql

INCLUDEPATH += $$PWD

HEADERS += $$PWD/../include/Qdbup/qdbup.h \
    $$PWD/qdbuptable.h \
    $$PWD/qdbupdatabase.h \
    $$PWD/genericdatabase.h \
    $$PWD/metatable.h \
    $$PWD/postgresdatabase.h \
    $$PWD/querybuilder.h \
    $$PWD/postgresquerybuilder.h \
    $$PWD/queryselect.h

SOURCES += \
    $$PWD/qdbuptable.cpp \
    $$PWD/qdbupdatabase.cpp \
    $$PWD/genericdatabase.cpp \
    $$PWD/metatable.cpp \
    $$PWD/postgresdatabase.cpp \
    $$PWD/querybuilder.cpp \
    $$PWD/postgresquerybuilder.cpp \
    $$PWD/queryselect.cpp
