include(lua_dependency.pri)

QT += quick \
    quickcontrols2  \
    widgets \
    core

CONFIG += c++14

CONFIG += qmltypes
QML_IMPORT_NAME = QuizImageDemoArtefactStepsFB
QML_IMPORT_MAJOR_VERSION = 1


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../MastactvaBase/quickframebufferobjects.cpp \
        drawing_tests.cpp \
        main.cpp \
        object_tests.cpp \
        quizimage.cpp

unix: SOURCES +=    \
    quizimagedrawingdata.cpp    \
    quizimageopengldrawingdata.cpp \
    opengldrawing_calculations.cpp  \
    effectargument_data.cpp \
    effectarg_data.cpp  \
    effectargvalue_data.cpp \
    effectargset_data.cpp   \
    effect_data.cpp \
    effectobjects_data.cpp  \
    objectinfo_data.cpp \
    objectartefact_data.cpp \
    artefactargtype_data.cpp    \
    artefactargstorage_data.cpp \
    artefacttype_data.cpp   \
    artefact_data.cpp    \
    artefactarg_data.cpp    \
    drawingdata_utils.cpp   \
    drawingdata_artefact.cpp    \
    drawingdata_artefactarg.cpp \
    drawingdata_effectobjects.cpp   \
    drawingdata_objectartefact.cpp  \
    drawingdata_objectinfo.cpp  \
    drawingdata_effect.cpp  \
    drawingdata_effectarg.cpp   \
    drawingdata_effectargvalue.cpp  \
    drawingdata_effectargset.cpp    \
    drawingdata_argsetsandargs.cpp  \
    data_utils.cpp  \
    mergeid.cpp \
    wavefrontobj.cpp    \
    luaapi.cpp  \
    addon.cpp   \
    addonmodule.cpp \
    addonmodules.cpp    \
    addonmodulelist.cpp \
    opengldrawing_utils.cpp \
    utils.cpp   \


HEADERS += \
    ../MastactvaBase/quickframebufferobjects.h \
    drawing_tests.h \
    object_tests.h \
    quizimage.h

unix: HEADERS +=    \
    quizimagedrawingdata.h  \
    quizimageopengldrawingdata.h \
    opengldrawing_calculations.h    \
    effectargument_data.h   \
    effectarg_data.h    \
    effectargvalue_data.h   \
    effectargset_data.h \
    effect_data.h   \
    effectobjects_data.h    \
    objectinfo_data.h   \
    objectartefact_data.h   \
    artefacttype_data.h \
    artefactargtype_data.h  \
    artefactargstorage_data.h   \
    artefact_data.h  \
    artefactarg_data.h  \
    drawingdata_utils.h \
    drawingdata_artefact.h  \
    drawingdata_artefactarg.h   \
    drawingdata_effectobjects.h \
    drawingdata_objectartefact.h    \
    drawingdata_objectinfo.h    \
    drawingdata_effect.h    \
    drawingdata_effectarg.h \
    drawingdata_effectargvalue.h    \
    drawingdata_effectargset.h  \
    drawingdata_argsetsandargs.h    \
    data_utils.h    \
    mergeid.h   \
    wavefrontobj.h  \
    lua_utils.h \
    luaapi.h    \
    addon.h \
    addonmodule.h   \
    addonmodules.h  \
    addonmodulelist.h   \
    opengldrawing_utils.h   \
    utils.h \


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
    todo.txt
