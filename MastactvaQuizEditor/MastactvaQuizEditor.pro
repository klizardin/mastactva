QT += quick \
    quickcontrols2  \
    widgets \
    sql

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuizEditor
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
        appconsts.h \
        galleryeditviewmodel.h    \
    localdataset.h \
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
        questionanswer.h    \
        localdata.h \
        requestdata.h   \
        localdataapinocache.h   \
        localdataapicache.h \
        userstep.h  \
        dbrequestinfo.h


SOURCES += \
        appconsts.cpp \
    localdataset.cpp \
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
        questionanswer.cpp  \
        localdata.cpp   \
        requestdata.cpp \
        localdataapinocache.cpp \
        localdataapicache.cpp   \
        userstep.cpp    \
        dbrequestinfo.cpp


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

