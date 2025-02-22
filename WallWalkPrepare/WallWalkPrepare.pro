QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp    \
        addonmodulelist.cpp \
        utils.cpp   \


HEADERS += \
        addonmodulelist.h   \
        utils.h \
        utils_json.h    \
        utils_enum.h    \
        utils_optional.h    \


TRANSLATIONS += \
    WallWalkPrepare_be_BY.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
