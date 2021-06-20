include(gtest_dependency.pri)
include(lua_dependency.pri)

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

