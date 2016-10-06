!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/ConnectorPluginManager.h

    SOURCES += \
        $$PWD/ConnectorPluginManager.cpp

    RESOURCES += \
        $$PWD/ConnectorPluginManager.qrc
}
