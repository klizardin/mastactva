QT += quick
QT += quickcontrols2

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuizEditor
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        appconsts.h \
        galleryeditviewmodel.h    \
        mastactvaapi.h    \
        netapiv0.h \
        qmlmainobjects.h \

unix: HEADERS += \
        easingtype.h \
        effect.h \
        effectarg.h \
        effectargset.h \
        effectargvalue.h \
        effectshader.h \
        shader.h \
        shaderargtype.h \
        shadertype.h    \
        utils.h \
        netappconsts.h \
        IModel.h   \
        qmlobjects.h   \
        imagesource.h  \
        Layout.h   \
        netapi.h   \
        Model.h \
        serverfiles.h   \
        quizimage.h \
        openglquizimage.h   \
        imagepointeffect.h  \
        gallery.h   \
        image.h \
        imagepoint.h    \
        imagedescription.h  \
        gallerystatistics.h \
        question.h  \
        answer.h    \
        questionanswer.h


SOURCES += \
        appconsts.cpp \
        main.cpp    \
        galleryeditviewmodel.cpp    \
        mastactvaapi.cpp    \
        netapiv0.cpp \
        qmlmainobjects.cpp  \

unix: SOURCES += \
        easingtype.cpp \
        effect.cpp \
        effectarg.cpp \
        effectargset.cpp \
        effectargvalue.cpp \
        effectshader.cpp \
        shader.cpp \
        shaderargtype.cpp \
        shadertype.cpp  \
        utils.cpp  \
        netappconsts.cpp  \
        IModel.cpp \
        qmlobjects.cpp \
        imagesource.cpp    \
        netapi.cpp \
        Model.cpp   \
        serverfiles.cpp \
        quizimage.cpp   \
        openglquizimage.cpp \
        imagepointeffect.cpp    \
        gallery.cpp   \
        image.cpp   \
        imagepoint.cpp    \
        imagedescription.cpp    \
        gallerystatistics.cpp   \
        question.cpp    \
        answer.cpp  \
        questionanswer.cpp


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
    ../MastactvaShaders/MoveImages/Vertex/mix_clock2.vsh \
    ../MastactvaShaders/MoveImages/Vertex/translate_scale_first.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_rectangles.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_rectangles_3state.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse1.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse2.vsh \
    ../MastactvaShaders/MoveImages/Vertex/mix_eclipse3.vsh \
    todo.txt

ANDROID_ABIS = armeabi-v7a

