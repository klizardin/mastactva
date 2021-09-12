include(lua_dependency.pri)

_QT += quick
QT += quickcontrols2 \
    quick   \
    sql

DEFINES += "LOCALDATAAPICACHE=1"
DEFINES += "SERVERFILES_CPP=1"
DEFINES += "QMLOBJECTS_CPP=1"

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuiz
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++14


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mastactvaapi.cpp \
        quizqmlobjects.cpp \

unix: SOURCES += \
        IModel.cpp \
        Model.cpp \
        imagesource.cpp \
        netapi.cpp \
        netappconsts.cpp \
        qmlobjects.cpp \
        utils.cpp  \
        serverfiles.cpp \
        localdata.cpp   \
        requestdata.cpp \
        localdataapinocache.cpp \
        localdataapinocache_default.cpp \
        localdataapicache.cpp   \
        dbrequestinfo.cpp   \
        localdataapiviews.cpp   \
        modelhelpers.cpp    \
        wavefrontobj.cpp    \
        dbutils.cpp \
        jsonutils.cpp   \
        sqlutils.cpp    \
        timeutils.cpp   \
        containerutils.cpp  \
        format.cpp  \
        data_utils.cpp  \
        luaapi.cpp  \
        opengldrawing_utils.cpp \
        \
        quizimage.cpp   \
        quizimagedrawingdata.cpp    \
        quizimageopengldrawingdata.cpp  \
        drawingdata_utils.cpp   \
        drawingdata_artefact.cpp    \
        drawingdata_artefactarg.cpp \
        drawingdata_effectobjects.cpp   \
        drawingdata_objectartefact.cpp  \
        drawingdata_objectinfo.cpp  \
        drawingdata_effect.cpp  \
        drawingdata_effectarg.cpp   \
        drawingdata_effectargvalue.cpp  \
        \
        question.cpp \
        questionanswer.cpp \
        gallerystatistics.cpp \
        image.cpp \
        imagedescription.cpp \
        imagepoint.cpp \
        answer.cpp \
        gallery.cpp \
        quizuser.cpp \
        userstep.cpp  \
        easingtype.cpp    \
        effectarg_data.cpp  \
        effectarg.cpp \
        effectargset_data.cpp   \
        effectargset.cpp  \
        effectargvalue_data.cpp \
        effectargvalue.cpp    \
        effect_data.cpp \
        effect.cpp    \
        imagepointeffect.cpp    \
        artefact_data.cpp   \
        artefact.cpp    \
        artefacttype_data.cpp   \
        artefacttype.cpp    \
        artefactargtype_data.cpp    \
        artefactargtype.cpp \
        artefactargstorage_data.cpp \
        artefactargstorage.cpp  \
        artefactarg_data.cpp    \
        artefactarg.cpp \
        translation.cpp \
        objectinfo_data.cpp \
        objectinfo.cpp    \
        objectartefact_data.cpp \
        objectartefact.cpp  \
        effectobjects_data.cpp  \
        effectobjects.cpp   \
        gallerymodelview.cpp    \
        questionanswermodelview.cpp \


RESOURCES += qml.qrc \
    Mastactva/

TRANSLATIONS += \
    MastactvaQuiz_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $${PWD}

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android {
    DEFINES += "ASSETS_ROOT=1"
    DEFINES += "ANDROID_FULLSCREEN=1"

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

    image_files.path = /assets/images/
    image_files.files = $$files($$PWD/../MastactvaData/images/*)
    INSTALLS += image_files
    artefacts_files.path = /assets/artefacts/
    artefacts_files.files = $$files($$PWD/../MastactvaData/artefacts/*)
    INSTALLS += artefacts_files
    dbase_files.path = /assets/
    dbase_files.files += $$PWD/../MastactvaData/mastactva_ro.db3
    dbase_files.files += $$PWD/../MastactvaData/mastactva_rw.db3
    INSTALLS += dbase_files
    translate_files.path = /assets/
    translate_files.files += $$PWD/../MastactvaData/MastactvaQuiz_be_BY.qm
    INSTALLS += translate_files
}

ANDROID_ABIS = armeabi-v7a


HEADERS += \
    mastactvaapi.h \
    quizqmlobjects.h \

unix: HEADERS += \
    IModel.h \
    Model.h \
    imagesource.h \
    Layout.h \
    netapi.h \
    netappconsts.h \
    qmlobjects.h \
    utils.h    \
    serverfiles.h   \
    localdata.h \
    requestdata.h   \
    localdataapinocache.h   \
    localdataapinocache_default.h   \
    localdataapicache.h \
    dbrequestinfo.h \
    localdataapiviews.h \
    modelhelpers.h  \
    names.h \
    wavefrontobj.h  \
    dbutils.h   \
    jsonutils.h \
    layout_enums.h  \
    layout_json.h   \
    layout_type.h   \
    layout_variant.h    \
    sqlutils.h  \
    timeutils.h \
    defines.h   \
    format.h    \
    format_type.h   \
    containerutils.h    \
    data_utils.h    \
    luaapi.h    \
    opengldrawing_utils.h   \
    \
    quizimage.h \
    quizimagedrawingdata.h  \
    quizimageopengldrawingdata.h    \
    drawingdata_utils.h \
    drawingdata_artefact.h  \
    drawingdata_artefactarg.h   \
    drawingdata_effectobjects.h \
    drawingdata_objectartefact.h    \
    drawingdata_objectinfo.h    \
    drawingdata_effect.h    \
    drawingdata_effectarg.h \
    drawingdata_effectargvalue.h    \
    \
    answer.h \
    gallery.h \
    gallerystatistics.h \
    image.h \
    imagedescription.h \
    imagepoint.h \
    question.h \
    questionanswer.h \
    quizuser.h \
    userstep.h  \
    easingtype.h    \
    effectarg_data.h    \
    effectarg.h \
    effectargset_data.h \
    effectargset.h  \
    effectargvalue_data.h   \
    effectargvalue.h    \
    effect_data.h   \
    effect.h    \
    imagepointeffect.h  \
    gallerymodelview.h  \
    questionanswermodelview.h   \
    artefact_data.h \
    artefact.h  \
    artefacttype_data.h \
    artefacttype.h    \
    artefactargtype_data.h  \
    artefactargtype.h   \
    artefactargstorage_data.h   \
    artefactargstorage.h    \
    artefactarg_data.h  \
    artefactarg.h \
    translation.h   \
    objectinfo_data.h   \
    objectinfo.h  \
    objectartefact_data.h   \
    objectartefact.h    \
    effectobjects_data.h    \
    effectobjects.h \


DISTFILES += \
    ../build-MastactvaQuiz-Desktop_Qt_5_15_1_GCC_64bit-Debug/mastactvaquiz_metatypes.json \
    ../build-MastactvaQuiz-Android_Qt_5_15_1_Clang_Multi_Abi-Debug/mastactvaquiz_metatypes.json    \
    GalleryFunctions.js \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    todo.txt
