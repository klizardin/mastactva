include(gtest_dependency.pri)


QT += core testlib sql
QT -= gui

TARGET = UnitTests

TEMPLATE = app
CONFIG += console c++14 warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += thread


unix: QMAKE_CXXFLAGS_WARN_ON -= -Wall
unix: QMAKE_CXXFLAGS_WARN_ON += -Wpedantic
unix: QMAKE_CXXFLAGS_WARN_ON += -Waddress
unix: QMAKE_CXXFLAGS_WARN_ON += -Warray-bounds=1
unix: QMAKE_CXXFLAGS_WARN_ON += -Wbool-compare
unix: QMAKE_CXXFLAGS_WARN_ON += -Wbool-operation
unix: QMAKE_CXXFLAGS_WARN_ON += -Wc++11-compat  -Wc++14-compat
unix: QMAKE_CXXFLAGS_WARN_ON += -Wcatch-value
unix: QMAKE_CXXFLAGS_WARN_ON += -Wchar-subscripts
unix: QMAKE_CXXFLAGS_WARN_ON += -Wcomment
unix: QMAKE_CXXFLAGS_WARN_ON += -Wenum-compare
unix: QMAKE_CXXFLAGS_WARN_ON += -Wformat
unix: QMAKE_CXXFLAGS_WARN_ON += -Wformat-overflow
unix: QMAKE_CXXFLAGS_WARN_ON += -Wformat-truncation
unix: QMAKE_CXXFLAGS_WARN_ON += -Wint-in-bool-context
unix: QMAKE_CXXFLAGS_WARN_ON += -Winit-self
unix: QMAKE_CXXFLAGS_WARN_ON += -Wlogical-not-parentheses
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmain
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmaybe-uninitialized
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmemset-elt-size
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmemset-transposed-args
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmisleading-indentation
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmissing-attributes
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmissing-braces
unix: QMAKE_CXXFLAGS_WARN_ON += -Wmultistatement-macros
unix: QMAKE_CXXFLAGS_WARN_ON += -Wnarrowing
unix: QMAKE_CXXFLAGS_WARN_ON += -Wnonnull
unix: QMAKE_CXXFLAGS_WARN_ON += -Wnonnull-compare
unix: QMAKE_CXXFLAGS_WARN_ON += -Wopenmp-simd
unix: QMAKE_CXXFLAGS_WARN_ON += -Wparentheses
unix: QMAKE_CXXFLAGS_WARN_ON += -Wpessimizing-move
unix: QMAKE_CXXFLAGS_WARN_ON += -Wreorder
unix: QMAKE_CXXFLAGS_WARN_ON += -Wrestrict
unix: QMAKE_CXXFLAGS_WARN_ON += -Wreturn-type
unix: QMAKE_CXXFLAGS_WARN_ON += -Wsequence-point
unix: QMAKE_CXXFLAGS_WARN_ON += -Wsign-compare
unix: QMAKE_CXXFLAGS_WARN_ON += -Wsizeof-pointer-div
unix: QMAKE_CXXFLAGS_WARN_ON += -Wsizeof-pointer-memaccess
unix: QMAKE_CXXFLAGS_WARN_ON += -Wstrict-aliasing
unix: QMAKE_CXXFLAGS_WARN_ON += -Wstrict-overflow=1
unix: QMAKE_CXXFLAGS_WARN_ON += -Wswitch
unix: QMAKE_CXXFLAGS_WARN_ON += -Wtautological-compare
unix: QMAKE_CXXFLAGS_WARN_ON += -Wtrigraphs
unix: QMAKE_CXXFLAGS_WARN_ON += -Wuninitialized
unix: QMAKE_CXXFLAGS_WARN_ON += -Wunknown-pragmas
unix: QMAKE_CXXFLAGS_WARN_ON += -Wunused-function
unix: QMAKE_CXXFLAGS_WARN_ON += -Wunused-label
unix: QMAKE_CXXFLAGS_WARN_ON += -Wunused-value
unix: QMAKE_CXXFLAGS_WARN_ON += -Wvolatile-register-var
unix: QMAKE_CXXFLAGS_WARN_OFF += -Wunused-variable


HEADERS += \
        format_unittests.h \
        tst_testcanasserttrue.h

unix: HEADERS += \
        dbutils.h   \
        dbutils_unittests.h \
        timeutils.h \


SOURCES += \
        main.cpp

unix: SOURCES += \
        dbutils.cpp \
        timeutils.cpp   \
