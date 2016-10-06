!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui

    !include($$PWD/../../PluginInterface/UrgBenriPluginInterface.pri) {
            error("Unable to include Plugin Interface.")
    }

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/PluginManager.h \
        $$PWD/PluginDialog.h \
        $$PWD/PluginInfo.h

    SOURCES += \
        $$PWD/PluginManager.cpp \
        $$PWD/PluginDialog.cpp \
        $$PWD/PluginInfo.cpp

    RESOURCES += \
        $$PWD/PluginManager.qrc
}
