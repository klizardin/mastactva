_QT += quick
QT += quickcontrols2

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuiz
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../MastactvaBase/IModel.cpp \
        ../MastactvaBase/Model.cpp \
        answer.cpp \
        appconsts.cpp \
        gallery.cpp \
        gallerystatistics.cpp \
        image.cpp \
        imagedescription.cpp \
        imagepoint.cpp \
        ../MastactvaBase/imagesource.cpp \
        main.cpp \
        mastactvaapi.cpp \
        ../MastactvaBase/netapi.cpp \
        ../MastactvaBase/qmlobjects.cpp \
        question.cpp \
        questionanswer.cpp \
        quizqmlobjects.cpp \
        quizuser.cpp \
        userstep.cpp \
        ../MastactvaBase/utils.cpp

RESOURCES += qml.qrc \
    Mastactva/

TRANSLATIONS += \
    MastactavaQuiz_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_ABIS = armeabi-v7a


HEADERS += \
    ../MastactvaBase/IModel.h \
    ../MastactvaBase/Layout.h \
    ../MastactvaBase/Model.h \
    answer.h \
    appconsts.h \
    gallery.h \
    gallerystatistics.h \
    image.h \
    imagedescription.h \
    imagepoint.h \
    ../MastactvaBase/imagesource.h \
    mastactvaapi.h \
    ../MastactvaBase/netapi.h \
    ../MastactvaBase/qmlobjects.h \
    question.h \
    questionanswer.h \
    quizqmlobjects.h \
    quizuser.h \
    userstep.h \
    ../MastactvaBase/utils.h

DISTFILES += \
    ../build-MastactavaQuiz-Desktop_Qt_5_15_1_GCC_64bit-Debug/mastactavaquiz_metatypes.json \
    GalleryFunctions.js \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    todo.txt

