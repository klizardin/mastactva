QT += quick
QT += quickcontrols2

CONFIG += qmltypes
QML_IMPORT_NAME = ShaderTest
QML_IMPORT_MAJOR_VERSION = 1

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        openglquizimage.h \
        quizimage.h \
        utils.h

SOURCES += \
        main.cpp    \
        openglquizimage.cpp \
        quizimage.cpp   \
        utils.cpp

RESOURCES += qml.qrc    \
    ./ShaderTest/

TRANSLATIONS += \
    ShaderTest_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../build-ShaderTest-Desktop_Qt_5_15_1_GCC_64bit-Debug/shadertest_metatypes.json \
    ./ShaderTest/Constants.qml \
    ./ShaderTest/qmldir
