include(lua_dependency.pri)

QT += quick \
    quickcontrols2  \
    widgets \
    core    \
    qml \
    opengl


CONFIG += c++14
CONFIG += qmltypes

QML_IMPORT_NAME = QuizImageDemoArtefactStepsFB
QML_IMPORT_MAJOR_VERSION = 1


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    drawing_tests.cpp \
    main.cpp    \
    object_tests.cpp    \
    quizimage.cpp   \
    tests/logoGeometry.cpp


unix: SOURCES +=    \
    addon.cpp   \
    addonmodule.cpp \
    addonmodulelist.cpp \
    addonmodules.cpp    \
    artefact_data.cpp   \
    artefactarg_data.cpp    \
    artefactargstorage_data.cpp \
    artefactargtype_data.cpp    \
    artefacttype_data.cpp       \
    data_utils.cpp  \
    defaulttexturerender.cpp    \
    drawingdata_argsetsandargs.cpp  \
    drawingdata_artefact.cpp    \
    drawingdata_artefactarg.cpp \
    drawingdata_effect.cpp      \
    drawingdata_effectarg.cpp   \
    drawingdata_effectargset.cpp    \
    drawingdata_effectargvalue.cpp  \
    drawingdata_effectobjects.cpp   \
    drawingdata_objectartefact.cpp  \
    drawingdata_objectinfo.cpp      \
    drawingdata_utils.cpp   \
    effect_data.cpp \
    effectarg_data.cpp  \
    effectargset_data.cpp   \
    effectargument_data.cpp \
    effectargvalue_data.cpp \
    effectobjects_data.cpp  \
    luaapi.cpp  \
    mergeid.cpp \
    objectartefact_data.cpp \
    objectinfo_data.cpp \
    opengldrawing_calculations.cpp  \
    opengldrawing_utils.cpp \
    quizimagedrawingdata.cpp    \
    quizimageopengldrawingdata.cpp  \
    quizimageqwindowsinglethread.cpp    \
    utils_comment.cpp   \
    utils_opengl.cpp    \
    utils_scaledtime.cpp    \
    utils.cpp   \
    wavefrontobj.cpp


HEADERS += \
    drawing_tests.h \
    object_tests.h  \
    quizimage.h \
    tests/logoGeomerty.h


unix: HEADERS +=    \
    addon.h \
    addonmodule.h   \
    addonmodulelist.h   \
    addonmodules.h  \
    artefact_data.h \
    artefactarg_data.h  \
    artefactargstorage_data.h   \
    artefactargtype_data.h  \
    artefacttype_data.h \
    data_utils.h    \
    defaulttexturerender.h  \
    drawingdata_argsetsandargs.h    \
    drawingdata_artefact.h  \
    drawingdata_artefactarg.h   \
    drawingdata_effect.h    \
    drawingdata_effectarg.h \
    drawingdata_effectargset.h  \
    drawingdata_effectargvalue.h    \
    drawingdata_effectobjects.h \
    drawingdata_objectartefact.h    \
    drawingdata_objectinfo.h    \
    drawingdata_utils.h \
    effect_data.h   \
    effectarg_data.h    \
    effectargset_data.h \
    effectargument_data.h   \
    effectargvalue_data.h   \
    effectobjects_data.h    \
    iquizimageqwindow.h \
    lua_utils.h \
    luaapi.h    \
    mergeid.h   \
    objectartefact_data.h   \
    objectinfo_data.h   \
    opengldrawing_calculations.h    \
    opengldrawing_utils.h   \
    quizimagedrawingdata.h  \
    quizimageopengldrawingdata.h    \
    quizimageqwindowsinglethread.h  \
    utils_comment.h \
    utils_enum.h    \
    utils_json.h    \
    utils_opengl.h  \
    utils_optional.h    \
    utils_scaledtime.h  \
    utils.h \
    wavefrontobj.h

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
    data_structures.txt \
    tmp.txt \
    todo.txt
