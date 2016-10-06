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

#ifndef CONNECTORSELECTORWIDGET_H
#define CONNECTORSELECTORWIDGET_H

#include <QTabWidget>
#include <QAction>
#include <QToolButton>
#include <QSettings>

#include <functional>

#include "PluginDataStructure.h"
#include "ConnectorPluginInterface.h"
#include "PluginInformation.h"

struct ConnectorState{
    bool connected;
    bool started;
    bool paused;
    ConnectorState(bool connected_ = false
            , bool started_ = false
            , bool paused_ = false)
        : connected(connected_)
        , started(started_)
        , paused(paused_){}
};

class ConnectorPluginManager : public QTabWidget
{
    Q_OBJECT
public:
    explicit ConnectorPluginManager(QWidget *parent = 0);
    virtual ~ConnectorPluginManager();

    int connectedCount();
    QVector<PluginInformation> pluginInformation();

    ConnectorState connectorState(const QString &id) const;

    void loadFile(const QString &filename);
    void loadPlugins(const QVector<ConnectorPluginInterface *> &sources, PluginManagerInterface *manager);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);

    int pluginCount() const{ return m_plugins.size(); }

signals:
    void currentPluginChanged(const QString &id);
    void pluginAdded(const QString &id);
    void pluginRemoved(const QString &id);
    void fileOpened(const QString &filename);

    void connectorStarted(const QString &id);
    void connectorStopped(const QString &id);
    void connectorPaused(const QString &id);

    void connectorConnected(const QString &id);
    void connectorDisconnected(const QString &id);

    void connectorDataReady(const QString &id
                            , const PluginDataStructure &data);

    void error(const QString &title, const QString &message);
    void information(const QString &title, const QString &message);
    void warning(const QString &title, const QString &message);

    void languageChanged();

public slots:
    void startAll();
    void pauseAll();
    void stopAll();

    void start();
    void pause();
    void stop();

protected:
    void changeEvent(QEvent* e);

private slots:
    void tabsChanged(int tabIndex);
    void tabToClose(int tabIndex);
    void connexionReady();
    void connexionLost();
    void measurementDataReady(const PluginDataStructure &data);
    void onConnectorStarted();
    void onConnectorStopped();
    void onConnectorPaused();

private:
    QVector<ConnectorPluginInterface *> m_plugins;
    QMap<QString, GeneralPluginInterface *> m_dictionary;
    PluginManagerInterface *m_pluginManager;

    QMenu *m_addMenu;
    QToolButton* m_addButton;

    void initAddActions();
    void initPluginTabs();
    void addPluginWidget(ConnectorPluginInterface *pw);
    void connectPluginWidget(ConnectorPluginInterface *pw);
    QString pluginTabname(GeneralPluginInterface *pw);
    QString pluginId(GeneralPluginInterface *pw);
    QString pluginTitle(GeneralPluginInterface *pw);
};

#endif // CONNECTORSELECTORWIDGET_H

