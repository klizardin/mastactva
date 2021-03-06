include(gtest_dependency.pri)


QT += core testlib sql
QT -= gui

TARGET = UnitTests

TEMPLATE = app
CONFIG += console c++14 warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += thread


HEADERS += \
        format_unittests.h \
        test_utils.h \
        tst_testcanasserttrue.h

unix: HEADERS += \
        dbutils.h   \
        localdataapinocache_default.h   \
        dbutils_unittests.h \
        timeutils.h \
        format.h    \
        format_type.h   \


SOURCES += \
        main.cpp

unix: SOURCES += \
        dbutils.cpp \
        localdataapinocache_default.cpp \
        timeutils.cpp   \
        format.cpp  \
