!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/MovingAverage.h \
        $$PWD/ViewerPluginManager.h

    SOURCES += \
        $$PWD/ViewerPluginManager.cpp

    FORMS += \
        $$PWD/ViewerPluginManager.ui

    RESOURCES += \
        $$PWD/viewerpluginmanager.qrc
}
