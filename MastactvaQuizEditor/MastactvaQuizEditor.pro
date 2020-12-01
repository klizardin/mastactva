QT += quick
QT += quickcontrols2

CONFIG += qmltypes
QML_IMPORT_NAME = MastactvaQuizEditor
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++14


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        appconsts.h \
        easingtype.h \
        effect.h \
        effectarg.h \
        effectargset.h \
        effectargvalue.h \
        effectshader.h \
        galleryeditviewmodel.h    \
        mastactvaapi.h    \
        netapiv0.h \
        qmlmainobjects.h \
        shader.h \
        shaderargtype.h \
        shadertype.h

unix: HEADERS += \
        utils.h \
        netappconsts.h \
        IModel.h   \
        qmlobjects.h   \
        imagesource.h  \
        Layout.h   \
        netapi.h   \
        Model.h


SOURCES += \
        appconsts.cpp \
        easingtype.cpp \
        effect.cpp \
        effectarg.cpp \
        effectargset.cpp \
        effectargvalue.cpp \
        effectshader.cpp \
        main.cpp    \
        galleryeditviewmodel.cpp    \
        mastactvaapi.cpp    \
        netapiv0.cpp \
        qmlmainobjects.cpp  \
        shader.cpp \
        shaderargtype.cpp \
        shadertype.cpp

unix: SOURCES += \
        utils.cpp  \
        netappconsts.cpp  \
        IModel.cpp \
        qmlobjects.cpp \
        imagesource.cpp    \
        netapi.cpp \
        Model.cpp


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
    ../EffectEditDialog.qml \
    ../build-MastactvaQuizEditor-Desktop_Qt_5_15_0_GCC_64bit-Debug/mastactvaquizeditor_metatypes.json \
    ../build-mastactvaquizeditor-webassembly/mastactvaquizeditor_metatypes.json \
    imports/MastactvaAPI/qmldir \
    Mastactva/Constants.qml \
    Mastactva/qmldir \
    todo.txt

ANDROID_ABIS = armeabi-v7a

