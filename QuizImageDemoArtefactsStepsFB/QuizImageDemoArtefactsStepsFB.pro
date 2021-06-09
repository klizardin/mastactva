QT += quick \
    quickcontrols2  \
    widgets \
    core

CONFIG += c++14

CONFIG += qmltypes
QML_IMPORT_NAME = QuizImageDemoArtefactStepsFB
QML_IMPORT_MAJOR_VERSION = 1


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

SOURCES += \
        main.cpp \
        quizimage.cpp

unix: SOURCES +=    \
    quizimagedrawingdata.cpp    \
    quizimageopengldrawingdata.cpp \
    effectobjects_data.cpp  \
    objectinfo_data.cpp \
    objectartefact_data.cpp \
    artefact_data.cpp    \
    artefactarg_data.cpp    \
    drawingdata_artefact.cpp    \
    drawingdata_artefactarg.cpp \
    drawingdata_effectobjects.cpp   \
    drawingdata_objectartefact.cpp  \
    drawingdata_objectinfo.cpp  \
    utils.cpp


HEADERS += \
    quizimage.h

unix: HEADERS +=    \
    quizimagedrawingdata.h  \
    quizimageopengldrawingdata.h \
    effectobjects_data.h    \
    objectinfo_data.h   \
    objectartefact_data.h   \
    artefact_data.h  \
    artefactarg_data.h  \
    drawingdata_artefact.h  \
    drawingdata_artefactarg.h   \
    drawingdata_effectobjects.h \
    drawingdata_objectartefact.h    \
    drawingdata_objectinfo.h    \
    utils.h


RESOURCES += qml.qrc    \
    Mastactva/

TRANSLATIONS += \
    QuizImageDemoArtefactsStepsFB_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD    \
    $${PWD}/../MastactvaBase/

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
