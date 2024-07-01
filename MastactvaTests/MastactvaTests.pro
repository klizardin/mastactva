include(gtest_dependency.pri)
include(lua_dependency.pri)

QT += core testlib sql quick
QT -= gui

TARGET = UnitTests

TEMPLATE = app
CONFIG += console c++14 warn_on depend_includepath testcase
CONFIG -= app_bundle
CONFIG += thread


HEADERS += \
        ../MastactvaBase/utils.h \
        ../MastactvaBase/utils_json.h \
        ../MastactvaBase/utils_enum.h \
        ../MastactvaBase/utils_optional.h \
        ../MastactvaBase/utils_scaledtime.h \
        addon_unittests.h \
        define_lua_types.h \
        format_unittests.h \
        lua_unittests.h \
        test_utils.h \
        tst_testcanasserttrue.h \
        utils_unittests.h

unix: HEADERS += \
        dbutils.h   \
        localdataapinocache_default.h   \
        dbutils_unittests.h \
        timeutils.h \
        format.h    \
        format_type.h   \
        lua_utils.h \
        luaapi.h    \
        addon.h \
        addonmodule.h   \
        addonmodules.h  \
        addonmodulelist.h   \
        drawingdata_utils.h \
        mergeid.h   \


SOURCES += \
        ../MastactvaBase/utils.cpp \
        ../MastactvaBase/utils_scaledtime.cpp \
        main.cpp

unix: SOURCES += \
        dbutils.cpp \
        localdataapinocache_default.cpp \
        timeutils.cpp   \
        format.cpp  \
        luaapi.cpp  \
        addon.cpp   \
        addonmodule.cpp \
        addonmodules.cpp    \
        addonmodulelist.cpp \
        drawingdata_utils.cpp   \
        mergeid.cpp \

