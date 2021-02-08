_QT += quick
QT += quickcontrols2 \
    quick   \
    sql

DEFINES += "LOCALDATAAPICACHE=1"

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuiz
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++14
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
        mastactvaapi.cpp \
        quizqmlobjects.cpp \

unix: SOURCES += \
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
        IModel.cpp \
        Model.cpp \
        imagesource.cpp \
        netapi.cpp \
        netappconsts.cpp \
        qmlobjects.cpp \
        utils.cpp  \
        serverfiles.cpp \
        quizimage.cpp   \
        openglquizimage.cpp \
        easingtype.cpp    \
        effectarg.cpp \
        effectargset.cpp  \
        effectargvalue.cpp    \
        effect.cpp    \
        imagepointeffect.cpp    \
        localdata.cpp   \
        requestdata.cpp \
        localdataapinocache.cpp \
        localdataapicache.cpp   \
        dbrequestinfo.cpp   \
        localdataapiviews.cpp   \
        gallerymodelview.cpp    \
        questionanswermodelview.cpp \
        quizimagedata.cpp   \
        artefact.cpp    \
        artefacttype.cpp    \
        artefactargtype.cpp \
        effectartefact.cpp  \
        effectartefactarg.cpp \
        effectartefactargset.cpp \
        translation.cpp \
        artefactargstorage.cpp


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
    IModel.h \
    Layout.h \
    Model.h \
    imagesource.h \
    netapi.h \
    netappconsts.h \
    qmlobjects.h \
    utils.h    \
    serverfiles.h   \
    quizimage.h \
    openglquizimage.h   \
    easingtype.h    \
    effectarg.h \
    effectargset.h  \
    effectargvalue.h    \
    effect.h    \
    imagepointeffect.h  \
    localdata.h \
    requestdata.h   \
    localdataapinocache.h   \
    localdataapicache.h \
    dbrequestinfo.h \
    localdataapiviews.h \
    gallerymodelview.h  \
    defines.h   \
    questionanswermodelview.h   \
    quizimagedata.h \
    artefact.h  \
    artefacttype.h    \
    artefactargtype.h \
    effectartefact.h    \
    effectartefactarg.h \
    effectartefactargset.h \
    translation.h   \
    artefactargstorage.h


DISTFILES += \
    ../build-MastactvaQuiz-Desktop_Qt_5_15_1_GCC_64bit-Debug/mastactvaquiz_metatypes.json \
    ../build-MastactvaQuiz-Android_Qt_5_15_1_Clang_Multi_Abi-Debug/mastactvaquiz_metatypes.json    \
    GalleryFunctions.js \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    todo.txt
