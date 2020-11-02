QT += quick

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        IModel.cpp \
        main.cpp \
        netapi.cpp \
        qmlobjects.cpp \
        quizuser.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    MastactavaQuiz_be_BY.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_ABIS = armeabi-v7a

HEADERS += \
    IModel.h \
    Layout.h \
    Model.h \
    netapi.h \
    qmlobjects.h \
    quizuser.h
