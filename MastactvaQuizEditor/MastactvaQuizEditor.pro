include(lua_dependency.pri)

QT += quick \
    quickcontrols2  \
    widgets \
    sql

DEFINES += "LOCALDATAAPINOCACHE=1"
DEFINES += "SERVERFILES_CPP=1"
DEFINES += "QMLOBJECTS_CPP=1"

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuizEditor
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++14


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        appconsts.h \
        galleryeditviewmodel.h    \
        localdataset.h \
        mastactvaapi.h    \
        netapiv0.h \
        qmlmainobjects.h \

unix: HEADERS += \
        IModel.h   \
        Model.h \
        imagesource.h  \
        Layout.h   \
        qmlobjects.h   \
        netapi.h   \
        localdata.h \
        requestdata.h   \
        localdataapinocache.h   \
        localdataapinocache_default.h   \
        localdataapicache.h \
        netappconsts.h \
        dbrequestinfo.h \
        localdataapiviews.h \
        modelhelpers.h  \
        wavefrontobj.h  \
        dbutils.h \
        jsonutils.h \
        layout_enums.h  \
        layout_json.h   \
        layout_type.h   \
        layout_variant.h    \
        sqlutils.h  \
        timeutils.h \
        format_type.h   \
        format.h    \
        serverfiles.h   \
        containerutils.h    \
        data_utils.h    \
        luaapi.h    \
        opengldrawing_utils.h   \
        utils.h \
        names.h \
        defines.h   \
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
        drawingdata_effectargset.h  \
        \
        easingtype.h \
        effect_data.h   \
        effect.h \
        effectargument_data.h   \
        effectarg_data.h    \
        effectarg.h \
        effectargset_data.h \
        effectargset.h \
        effectargvalue_data.h   \
        effectargvalue.h \
        imagepointeffect.h  \
        gallery.h   \
        image.h \
        imagepoint.h    \
        imagedescription.h  \
        gallerystatistics.h \
        question.h  \
        answer.h    \
        questionanswer.h    \
        userstep.h  \
        quizuser.h  \
        artefact_data.h \
        artefact.h    \
        artefacttype_data.h \
        artefacttype.h    \
        artefactargtype_data.h  \
        artefactargtype.h \
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


SOURCES += \
        appconsts.cpp \
        localdataset.cpp \
        main.cpp    \
        galleryeditviewmodel.cpp    \
        mastactvaapi.cpp    \
        netapiv0.cpp \
        qmlmainobjects.cpp  \

unix: SOURCES += \
        IModel.cpp \
        Model.cpp   \
        imagesource.cpp    \
        netapi.cpp \
        serverfiles.cpp \
        netappconsts.cpp  \
        qmlobjects.cpp \
        localdata.cpp   \
        requestdata.cpp \
        localdataapinocache.cpp \
        localdataapinocache_default.cpp \
        localdataapicache.cpp   \
        userstep.cpp    \
        dbrequestinfo.cpp   \
        localdataapiviews.cpp   \
        modelhelpers.cpp    \
        wavefrontobj.cpp    \
        dbutils.cpp \
        jsonutils.cpp \
        sqlutils.cpp    \
        format.cpp  \
        timeutils.cpp   \
        containerutils.cpp  \
        data_utils.cpp  \
        luaapi.cpp  \
        opengldrawing_utils.cpp \
        utils.cpp  \
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
        drawingdata_effectargset.cpp    \
        \
        easingtype.cpp \
        effect_data.cpp \
        effect.cpp \
        effectargument_data.cpp \
        effectarg_data.cpp  \
        effectarg.cpp \
        effectargset_data.cpp   \
        effectargset.cpp \
        effectargvalue_data.cpp \
        effectargvalue.cpp \
        imagepointeffect.cpp    \
        gallery.cpp   \
        image.cpp   \
        imagepoint.cpp    \
        imagedescription.cpp    \
        gallerystatistics.cpp   \
        question.cpp    \
        answer.cpp  \
        questionanswer.cpp  \
        quizuser.cpp    \
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


RESOURCES += qml.qrc    \
    Mastactva/


TRANSLATIONS += \
    MastactvaQuizEditor_be_BY.ts

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
    ../build-MastactvaQuizEditor-Desktop_Qt_5_15_0_GCC_64bit-Debug/mastactvaquizeditor_metatypes.json \
    ../build-mastactvaquizeditor-webassembly/mastactvaquizeditor_metatypes.json \
    architecture_marks.txt \
    bugs.txt \
    imports/MastactvaAPI/qmldir \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    ../MastactvaShaders/MoveImages/Fragment/position_color.fsh \
    ../MastactvaShaders/MoveImages/Fragment/default_view_two_images_mix.frag \
    ../MastactvaShaders/MoveImages/Fragment/default_view_two_images_mix_both.fsh \
    ../MastactvaShaders/MoveImages/Fragment/default_view_two_images_prefere_first.frag    \
    ../MastactvaShaders/MoveImages/Fragment/default_view_two_images_prefere_second.frag    \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t2_and.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t3_and.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t2_or.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t3_or.vsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t_colort.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t_1st_pxor_2nd.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t2_1st_pxor_2nd.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t3_1st_pxor_2nd.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t_2steps_through_color.fsh \
    ../MastactvaShaders/MoveImages/Fragment/mix_varying_t2_or_2steps_through_color.fsh \
    ../MastactvaShaders/MoveImages/Vertex/cube_rotation.vsh \
    ../MastactvaShaders/MoveImages/Vertex/cube_rotation_horizontal.vsh \
    ../MastactvaShaders/MoveImages/Vertex/cube_rotation_horizontal_faced.vsh \
    ../MastactvaShaders/MoveImages/Vertex/swipe.vert    \
    ../MastactvaShaders/MoveImages/Vertex/swipe_stick.vsh    \
    ../MastactvaShaders/MoveImages/Vertex/mix_direction.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_direction_faced4.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_direction_line2.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_line_horizontal.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_line_horizontal_faced4.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_clock.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_clock2.vsh \
    ../MastactvaShaders/MoveImages/Vertex/translate_scale_first.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_rectangles.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_rectangles_3state.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_rotated_rectangles.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse1.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse2.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse3.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse2_2steps.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse2x2_2steps.vsh \
    todo.txt

ANDROID_ABIS = armeabi-v7a
