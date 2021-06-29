include(gtest_dependency.pri)
include(lua_dependency.pri)

QT += core testlib sql gui

TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += thread

HEADERS += \
        luaapi.h \
        tst_installed.h \
        tst_luaapi.h

SOURCES += \
        luaapi.cpp \
        main.cpp

