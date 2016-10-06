/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>

#include "ToolPluginInterface.h"
#include "ViewerPluginManager.h"
#include "ConnectorPluginManager.h"
#include "SystemPluginInterface.h"
#include "FilterPluginManager.h"


#include <QTime>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QDragEnterEvent>
#include <QSettings>
#include <QProgressDialog>

#include "MutiViewerWidget.h"
#include "PluginDialog.h"
//#include "SettingsWindow.h"
#include "QRecentFilesMenu.h"
#include "PluginManager.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

signals:
    void languageChanged();

protected:
    void changeEvent(QEvent* e);
    void closeEvent(QCloseEvent*event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    Ui::MainWindow* ui;
    QTranslator translator;
    MutiViewerWidget *m_viewerManager;
    ConnectorPluginManager *m_connectorManager;
    FilterPluginManager *m_filterManager;
    ViewerPluginManager* viewer;
    PluginDialog *m_pluginDialog;
    QRecentFilesMenu* m_recentFilesMenu;
//    SettingsWindow m_settingsWindow;
    QTime acquisitionTimer;
    QSettings m_settings;
    QVector<ToolPluginInterface *> m_tools;
    QVector<SystemPluginInterface *> m_systems;

    bool m_pauseMode;

    void setupConnections();
    void restoreState();
    void saveState();
    void initLanguage();
    void setupShortcuts();
    void setupMenus();
    void initPlayControls();
    void updateViewerSources();
    void clearViewer(const QString &id);

private slots:
    void aboutButtonClicked();
    void loadToolsTranslator(const QString &locale);
    void loadSystemsTranslator(const QString &locale);
    void actionEnglishToggled(bool state);
    void actionFrancaisToggled(bool state);
    void actionJapaneseToggled(bool state);

    //=============================================
    void pluginAdded(const QString &id);
    void pluginRemoved(const QString &id);
    void updateControls(const QString &id);
    void switchViewer(const QString &id);
    void deviceDataReady(const QString &id, const PluginDataStructure &data);

    //--------------------------------------------
    void updatePlayControls(bool activeState, bool startedState, bool pausedState);

    void actionOpenTriggered();
    void loadFile(QString filename);
    void checkForLoadFile();
    void UBHFileFormat();

    void notifyError(const QString &header, const QString &message);
    void notifyWarning(const QString &header, const QString &message);
    void notifyInformation(const QString &header, const QString &message);
};

#endif // MAINWINDOW_H

