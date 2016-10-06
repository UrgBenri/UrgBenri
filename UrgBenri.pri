QT       += core gui network opengl xml concurrent

!include(libs/QRecentFilesMenu/QRecentFilesMenu.pri) {
        error("Unable to include QRecentFilesMenu library.")
}

!include(libs/MutiViewerWidget/MutiViewerWidget.pri) {
        error("Unable to include Multi ViewerWidget Widget.")
}

!include(libs/ConnectorPluginManager/ConnectorPluginManager.pri) {
        error("Unable to include Connector Plugin Manager.")
}

!include(libs/PlayControlsWidget/PlayControlsWidget.pri) {
        error("Unable to include Play Controls Widget.")
}

!include(libs/LogConsoleWidget/LogConsoleWidget.pri) {
        error("Unable to include Log Console Widget.")
}

!include(libs/FilterPluginManager/FilterPluginManager.pri) {
        error("Unable to include Filter Plugin Manager.")
}

!include(libs/PluginManager/PluginManager.pri) {
        error("Unable to include Plugin Manager.")
}

CONFIG += exceptions rtti

INCLUDEPATH += $$PWD

SOURCES += \
        $$PWD/MainWindow.cpp

HEADERS  += \
        $$PWD/MainWindow.h

FORMS    += \
        $$PWD/MainWindow.ui

RESOURCES += $$PWD/UrgBenri.qrc
