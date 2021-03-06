QT += quick
QT += quickcontrols2
QT += sql

CONFIG += qmltypes
QML_IMPORT_NAME = ShaderTest
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++17


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


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        openglquizimage.h \
        quizimage.h \
        utils.h

SOURCES += \
        main.cpp    \
        openglquizimage.cpp \
        quizimage.cpp   \
        utils.cpp

RESOURCES += qml.qrc    \
    ShaderTestQML/

TRANSLATIONS += \
    ShaderTest_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-ShaderTest-Desktop_Qt_5_15_1_GCC_64bit-Debug/shadertest_metatypes.json \
    ./ShaderTestQML/Constants.qml \
    ./ShaderTestQML/qmldir
