include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

HEADERS += \
        luaapi.h \
        tst_installed.h

SOURCES += \
        luaapi.cpp \
        main.cpp

# lua
INCLUDEPATH += /home/klizardin/projects/lua-5.4.3/src/
LIBS += -L/home/klizardin/projects/lua-5.4.3/src/ -llua
