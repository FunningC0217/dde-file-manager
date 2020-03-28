#-------------------------------------------------
#
# Project created by QtCreator 2017-04-20T15:40:08
#
#-------------------------------------------------

QT       += core gui widgets multimedia

TARGET = dde-music-preview-plugin
TEMPLATE = lib

PKGCONFIG += taglib

CONFIG += c++11 plugin link_pkgconfig

include(../../../common/common.pri)

SOURCES += \
    toolbarframe.cpp \
    musicmessageview.cpp \
    musicpreview.cpp \
    main.cpp

HEADERS += \
    toolbarframe.h \
    musicmessageview.h \
    musicpreview.h
DISTFILES += dde-music-preview-plugin.json

PLUGIN_INSTALL_DIR = $$PLUGINDIR/previews

DESTDIR = $$top_srcdir/plugins/previews

CONFIG(release, debug|release) {
    !system($$PWD/generate_translations.sh): error("Failed to generate translation")
}


unix {
    target.path = $$PLUGIN_INSTALL_DIR

    translations.path = $${PREFIX}/share/dde-file-manager/translations
    translations.files = $$PWD/translations/*.qm
    INSTALLS += target translations
}

RESOURCES += \
    dde-music-preview-plugin.qrc