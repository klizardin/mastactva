include(lua_dependency.pri)

QT += quick \
    quickcontrols2  \
    widgets \
    core    \
    qml \
    opengl  \
    sql

CONFIG += c++14 qmltypes
CONFIG -= app_bundle

QML_IMPORT_NAME = WallWalkDemoSimple
QML_IMPORT_MAJOR_VERSION = 1


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    drawingdatainitializer_utils.cpp    \
    drawingdatainitializer.cpp  \
    main.cpp    \
    quizimage.cpp   \


HEADERS += \
    drawingdatainitializer_utils.h  \
    drawingdatainitializer.h    \
    quizimage.h


unix:SOURCES += \
    addon.cpp   \
    artefact_data.cpp   \
    artefactarg_data.cpp    \
    artefactargstorage_data.cpp \
    artefactargtype_data.cpp    \
    artefacttype_data.cpp   \
    containerutils.cpp  \
    data_utils.cpp  \
    dbrequestinfo.cpp   \
    dbutils.cpp \
    defaulttexturerenderdemo.cpp    \
    drawingdata_argsetsandargsdemo.cpp  \
    drawingdata_artefactargdemo.cpp \
    drawingdata_artefactdemo.cpp    \
    drawingdata_effectargdemo.cpp   \
    drawingdata_effectargsetdemo.cpp    \
    drawingdata_effectargvaluedemo.cpp  \
    drawingdata_effectdemo.cpp  \
    drawingdata_effectobjectsdemo.cpp   \
    drawingdata_objectartefactdemo.cpp  \
    drawingdata_objectinfodemo.cpp  \
    drawingdata_utilsdemo.cpp   \
    effect_data.cpp \
    effectargument_data.cpp \
    effectobjects_data.cpp  \
    format.cpp  \
    imagesource.cpp \
    IModel.cpp  \
    localdata.cpp   \
    localdataapicache.cpp   \
    localdataapinocache_default.cpp \
    localdataapinocache.cpp \
    mergeid.cpp \
    Model.cpp   \
    modelconfig.cpp \
    netapi.cpp  \
    objectartefact_data.cpp \
    objectinfo_data.cpp \
    opengldrawing_calculations.cpp  \
    opengldrawing_utils.cpp \
    qmlobjects.cpp  \
    quizimagedrawingdatademo.cpp    \
    quizimageopengldrawingdatademo.cpp  \
    quizimageqwindowsinglethreaddemo.cpp    \
    requestdata.cpp \
    timeutils.cpp   \
    utils_scaledtime.cpp    \
    utils.cpp   \
    \
    wavefrontobj.cpp    \


unix:HEADERS += \
    addon.h \
    artefact_data.h \
    artefactarg_data.h  \
    artefactargstorage_data.h   \
    artefactargtype_data.h  \
    artefacttype_data.h \
    containerutils.h    \
    data_utils.h    \
    dbrequestinfo.h \
    dbutils.h   \
    defaulttexturerenderdemo.h  \
    defines.h   \
    drawingdata_argsetsandargsdemo.h    \
    drawingdata_artefactargdemo.h   \
    drawingdata_artefactdemo.h  \
    drawingdata_effectargdemo.h \
    drawingdata_effectargsetdemo.h  \
    drawingdata_effectargvaluedemo.h    \
    drawingdata_effectdemo.h    \
    drawingdata_effectobjectsdemo.h \
    drawingdata_objectartefactdemo.h    \
    drawingdata_objectinfodemo.h    \
    drawingdata_utilsdemo.h \
    effect_data.h   \
    effectargument_data.h   \
    effectobjects_data.h    \
    format_type.h   \
    format.h    \
    imagesource.h   \
    IModel.h    \
    iquizimageqwindow.h \
    layout_enums.h  \
    layout_json.h   \
    layout_type.h   \
    layout_variant.h    \
    Layout.h    \
    localdata.h \
    localdataapicache.h \
    localdataapinocache_default.h   \
    localdataapinocache.h   \
    mergeid.h   \
    Model.h \
    modelconfig.h   \
    names.h \
    netapi.h    \
    objectartefact_data.h   \
    objectinfo_data.h   \
    opengldrawing_calculations.h    \
    opengldrawing_utils.h   \
    qmlobjects.h    \
    quizimagedrawingdatademo.h  \
    quizimageopengldrawingdatademo.h    \
    quizimageqwindowsinglethreaddemo.h  \
    requestdata.h   \
    timeutils.h \
    utils_enum.h    \
    utils_json.h    \
    utils_optional.h    \
    utils_scaledtime.h  \
    utils.h \
    \
    wavefrontobj.h  \


RESOURCES += qml.qrc    \


TRANSLATIONS += \
    WallWalkDemoSimple_be_BY.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lua_dependency.pri \
    qml/quizImage.qml
