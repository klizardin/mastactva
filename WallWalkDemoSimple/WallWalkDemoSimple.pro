include(lua_dependency.pri)

QT += quick \
    quickcontrols2  \
    widgets \
    core    \
    qml \
    opengl

CONFIG += c++14 qmltypes
CONFIG -= app_bundle

QML_IMPORT_NAME = WallWalkDemoSimple
QML_IMPORT_MAJOR_VERSION = 1


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    drawingdatainitializer.cpp \
        main.cpp    \
        quizimage.cpp   \


HEADERS += \
    drawingdatainitializer.h \
        quizimage.h \


unix:SOURCES += \
    quizimageqwindowsinglethreaddemo.cpp    \
    defaulttexturerenderdemo.cpp    \
    quizimagedrawingdatademo.cpp    \
    drawingdata_utilsdemo.cpp   \


unix:HEADERS += \
    quizimageqwindowsinglethreaddemo.h  \
    defaulttexturerenderdemo.h  \
    iquizimageqwindow.h \
    quizimagedrawingdatademo.h  \
    drawingdata_utilsdemo.h \


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
