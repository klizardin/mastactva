_QT += quick
QT += quickcontrols2 \
    quick

CONFIG += c++17 qmltypes
QML_IMPORT_NAME = QuizImageDemo
QML_IMPORT_MAJOR_VERSION = 1

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        openglquizimage.cpp \
        quizimage.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    QuizImageDemo_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    openglquizimage.h \
    quizimage.h

DISTFILES += \
    ../build-QuizImageDemo-Desktop_Qt_5_15_1_GCC_64bit-Debug/quizimagedemo_metatypes.json \
    todo.txt

