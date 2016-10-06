!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    !include($$PWD/../QuadSplitter/QuadSplitter.pri) {
            error("Unable to include Quad Splitter Widget.")
    }

    !include($$PWD/../ViewerPluginManager/ViewerPluginManager.pri) {
            error("Unable to include Viewer Plugin Manager.")
    }


    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    SOURCES += \
            $$PWD/MutiViewerWidget.cpp

    HEADERS  += \
            $$PWD/MutiViewerWidget.h

    FORMS += \
            $$PWD/MutiViewerWidget.ui
    RESOURCES += \
            $$PWD/MutiViewerWidget.qrc
}

