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
        serverfiles.cpp


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
    serverfiles.h


DISTFILES += \
    ../build-MastactvaQuiz-Desktop_Qt_5_15_1_GCC_64bit-Debug/mastactvaquiz_metatypes.json \
    GalleryFunctions.js \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    todo.txt

