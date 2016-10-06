!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/FilterPluginManager.h

    SOURCES += \
        $$PWD/FilterPluginManager.cpp

    RESOURCES += \
        $$PWD/FilterPluginManager.qrc
}
