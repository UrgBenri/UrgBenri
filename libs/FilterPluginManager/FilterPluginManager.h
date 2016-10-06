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

#ifndef FILTERPLUGINMANAGER_H
#define FILTERPLUGINMANAGER_H

#include <QTabWidget>
#include <QComboBox>
#include <QStackedLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>

#include "ConnectorPluginInterface.h"
#include "FilterPluginInterface.h"

class FilterPluginManager: public QTabWidget
{
    Q_OBJECT

public:
    explicit FilterPluginManager(QWidget *parent = 0);
    virtual ~FilterPluginManager();

    void loadPlugins(const QVector<FilterPluginInterface *> &filters, PluginManagerInterface *manager);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);

    void updateConnectorsList(const QVector<PluginInformation> &infos);

    QVector<PluginInformation> pluginInformation();

    int pluginCount() const{ return m_plugins.size(); }

signals:
    void currentPluginChanged(const QString &id);
    void pluginAdded(const QString &id);
    void pluginRemoved(const QString &id);

    void connectorDataReady(const QString &id
                            , const PluginDataStructure &data);

    void error(const QString &title, const QString &message);
    void information(const QString &title, const QString &message);
    void warning(const QString &title, const QString &message);

    void languageChanged();

protected:
    void changeEvent(QEvent *e);

public slots:
    void onConnectorDataReady(const QString &id
                            , const PluginDataStructure &data);

private slots:
    void measurementDataReady(const PluginDataStructure &data);
    void tabToClose(int tabIndex);
    void tabsChanged(int tabIndex);

private:
    QVector<FilterPluginInterface *> m_plugins;
    QMap<QString, GeneralPluginInterface *> m_dictionary;
    PluginManagerInterface *m_pluginManager;
    QVector<PluginInformation> m_sources;

    QMenu *m_addMenu;
    QToolButton* m_addButton;

    void initAddActions();

    void initPluginTabs();
    void addPluginWidget(FilterPluginInterface *pw);
    void connectPluginWidget(FilterPluginInterface *pi);
    QString pluginTabname(GeneralPluginInterface *pw);
    QString pluginId(GeneralPluginInterface *pw);
    QString pluginTitle(GeneralPluginInterface *pw);
};

#endif // FILTERPLUGINMANAGER_H

