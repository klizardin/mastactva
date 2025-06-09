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
    drawingdatainitializer.cpp \
    drawingdatainitializer_utils.cpp \
    main.cpp    \
    quizimage.cpp   \


HEADERS += \
    drawingdatainitializer.h \
    drawingdatainitializer_utils.h \
    quizimage.h \


unix:SOURCES += \
    quizimageqwindowsinglethreaddemo.cpp    \
    defaulttexturerenderdemo.cpp    \
    quizimagedrawingdatademo.cpp    \
    drawingdata_utilsdemo.cpp   \
    effect_data.cpp \
    artefact_data.cpp   \
    IModel.cpp  \
    imagesource.cpp \
    mergeid.cpp \
    artefactarg_data.cpp    \
    artefactargtype_data.cpp    \
    artefactargstorage_data.cpp \
    effectargument_data.cpp \
    artefacttype_data.cpp   \
    utils.cpp   \
    qmlobjects.cpp  \
    netapi.cpp  \
    timeutils.cpp   \
    requestdata.cpp \
    localdata.cpp   \
    localdataapinocache.cpp \
    dbutils.cpp \
    format.cpp  \
    dbrequestinfo.cpp   \
    localdataapinocache_default.cpp \
    localdataapicache.cpp   \
    containerutils.cpp  \
    Model.cpp   \
    modelconfig.cpp \
    data_utils.cpp  \
    objectinfo_data.cpp \
    objectartefact_data.cpp \
    drawingdata_effectdemo.cpp  \
    drawingdata_argsetsandargsdemo.cpp  \
    drawingdata_effectobjectsdemo.cpp   \
    drawingdata_effectargsetdemo.cpp    \
    drawingdata_effectargdemo.cpp   \
    drawingdata_effectargvaluedemo.cpp  \
    drawingdata_artefactargdemo.cpp \
    addon.cpp   \
    drawingdata_objectinfodemo.cpp  \
    drawingdata_objectartefactdemo.cpp  \
    drawingdata_artefactdemo.cpp    \
    utils_scaledtime.cpp    \
    effectobjects_data.cpp  \
    quizimageopengldrawingdatademo.cpp  \


unix:HEADERS += \
    quizimageqwindowsinglethreaddemo.h  \
    defaulttexturerenderdemo.h  \
    iquizimageqwindow.h \
    quizimagedrawingdatademo.h  \
    drawingdata_utilsdemo.h \
    effect_data.h   \
    artefact_data.h \
    IModel.h    \
    imagesource.h   \
    mergeid.h   \
    layout_enums.h  \
    layout_variant.h    \
    layout_type.h   \
    layout_json.h   \
    artefactarg_data.h  \
    artefactargtype_data.h  \
    artefactargstorage_data.h   \
    effectargument_data.h   \
    artefacttype_data.h \
    utils.h \
    utils_json.h    \
    utils_enum.h    \
    utils_optional.h    \
    qmlobjects.h    \
    netapi.h    \
    Layout.h    \
    timeutils.h \
    requestdata.h   \
    localdata.h \
    localdataapinocache.h   \
    dbutils.h   \
    names.h \
    format.h    \
    format_type.h   \
    dbrequestinfo.h \
    localdataapinocache_default.h   \
    defines.h   \
    localdataapicache.h \
    containerutils.h    \
    Model.h \
    modelconfig.h   \
    data_utils.h    \
    objectinfo_data.h   \
    objectartefact_data.h   \
    drawingdata_effectdemo.h    \
    drawingdata_argsetsandargsdemo.h    \
    drawingdata_effectobjectsdemo.h \
    drawingdata_effectargsetdemo.h  \
    drawingdata_effectargdemo.h \
    drawingdata_effectargvaluedemo.h    \
    drawingdata_artefactargdemo.h   \
    addon.h \
    drawingdata_objectinfodemo.h    \
    drawingdata_objectartefactdemo.h    \
    drawingdata_artefactdemo.h  \
    utils_scaledtime.h  \
    effectobjects_data.h    \
    quizimageopengldrawingdatademo.h    \


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
