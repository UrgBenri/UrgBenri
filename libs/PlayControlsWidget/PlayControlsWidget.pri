!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui


    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD


    FORMS += \
        $$PWD/PlayControlsWidget.ui

    HEADERS += \
        $$PWD/PlayControlsWidget.h

    SOURCES += \
        $$PWD/PlayControlsWidget.cpp

    RESOURCES += \
        $$PWD/PlayControlsWidget.qrc
}
