QT += quick \
    quickcontrols2  \
    widgets \
    core

CONFIG += c++14

CONFIG += qmltypes
QML_IMPORT_NAME = QuizImageDemoArtefactStepsFB
QML_IMPORT_MAJOR_VERSION = 1


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        drawing_tests.cpp \
        main.cpp \
        object_tests.cpp \
        quizimage.cpp

unix: SOURCES +=    \
    quizimagedrawingdata.cpp    \
    quizimageopengldrawingdata.cpp \
    effect_data.cpp \
    effectobjects_data.cpp  \
    objectinfo_data.cpp \
    objectartefact_data.cpp \
    artefactargtype_data.cpp    \
    artefactargstorage_data.cpp \
    artefacttype_data.cpp   \
    artefact_data.cpp    \
    artefactarg_data.cpp    \
    drawingdata_utils.cpp   \
    drawingdata_artefact.cpp    \
    drawingdata_artefactarg.cpp \
    drawingdata_effectobjects.cpp   \
    drawingdata_objectartefact.cpp  \
    drawingdata_objectinfo.cpp  \
    drawingdata_effect.cpp  \
    data_utils.cpp  \
    wavefrontobj.cpp    \
    utils.cpp   \


HEADERS += \
    drawing_tests.h \
    object_tests.h \
    quizimage.h

unix: HEADERS +=    \
    quizimagedrawingdata.h  \
    quizimageopengldrawingdata.h \
    effect_data.h   \
    effectobjects_data.h    \
    objectinfo_data.h   \
    objectartefact_data.h   \
    artefacttype_data.h \
    artefactargtype_data.h  \
    artefactargstorage_data.h   \
    artefact_data.h  \
    artefactarg_data.h  \
    drawingdata_utils.h \
    drawingdata_artefact.h  \
    drawingdata_artefactarg.h   \
    drawingdata_effectobjects.h \
    drawingdata_objectartefact.h    \
    drawingdata_objectinfo.h    \
    drawingdata_effect.h    \
    data_utils.h    \
    wavefrontobj.h  \
    utils.h \


RESOURCES += qml.qrc    \
    Mastactva/

TRANSLATIONS += \
    QuizImageDemoArtefactsStepsFB_be_BY.ts

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
    Mastactva/Constants.qml \
    Mastactva/qmldir \
