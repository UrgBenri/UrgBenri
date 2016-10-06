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

#include "ConnectorPluginManager.h"

#include <QMenu>
#include <QMessageBox>
#include <QIcon>
#include <QTabBar>
#include <QDir>
#include <QFile>
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>

#include "PluginManager.h"

#define PLUGIN_INDEX_PROPERTY "PluginIndex"

ConnectorPluginManager::ConnectorPluginManager(QWidget *parent)
    : QTabWidget(parent)
    , m_addMenu(new QMenu(this))
    , m_addButton(new QToolButton(this))
{
    setTabsClosable(true);

    initAddActions();

    connect(this, &ConnectorPluginManager::currentChanged,
            this, &ConnectorPluginManager::tabsChanged);
    connect(this, &ConnectorPluginManager::tabCloseRequested,
            this, &ConnectorPluginManager::tabToClose);
}

ConnectorPluginManager::~ConnectorPluginManager()
{
}

void ConnectorPluginManager::loadPlugins(const QVector<ConnectorPluginInterface *> &sources, PluginManagerInterface *manager)
{
    m_plugins = sources;
    m_pluginManager = manager;
    initPluginTabs();
}

void ConnectorPluginManager::changeEvent(QEvent *e)
{
    QTabWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange: {
            emit languageChanged();
        }break;
        default: {
        }
    }
}

void ConnectorPluginManager::initAddActions()
{
    m_addButton->setIcon(QIcon(QStringLiteral(":/ConnectorPluginManager/add_icon")));
    m_addButton->setIconSize(QSize(24, 24));
    m_addButton->setMenu(m_addMenu);
    m_addButton->setPopupMode(QToolButton::InstantPopup);

    connect(this, &ConnectorPluginManager::languageChanged,
            this, [=](){
        m_addButton->setToolTip(tr("Add new Source"));
    });
    m_addButton->setToolTip(tr("Add new Source"));

    setCornerWidget(m_addButton, Qt::TopRightCorner);
}

void ConnectorPluginManager::initPluginTabs()
{
    foreach (ConnectorPluginInterface *pw, m_plugins) {
        if(pw){
            QAction *action = new QAction(m_addMenu);
            action->setText(pw->pluginName());
            action->setIcon(pw->pluginIcon());
            m_addMenu->addAction(action);
            connect(action, &QAction::triggered, this, [=](){
                ConnectorPluginInterface *pi = qobject_cast<ConnectorPluginInterface*>(pw->createPlugin(this));
                if(pi){
                    pi->onLoad(m_pluginManager);
                    addPluginWidget(pi);
                }
            });
            connect(this, &ConnectorPluginManager::languageChanged,
                    this, [=](){
                action->setText(pw->pluginName());
            });
            addPluginWidget(pw);
            tabBar()->setTabButton(tabBar()->count() -1, QTabBar::RightSide, 0);
            tabBar()->setTabButton(tabBar()->count() -1, QTabBar::LeftSide, 0);
        }
    }

    setCurrentIndex(0);
}

void ConnectorPluginManager::addPluginWidget(ConnectorPluginInterface *pw)
{
    int index = 0;
    for(int i = 0; i < count(); ++i){
        ConnectorPluginInterface *tw = qobject_cast<ConnectorPluginInterface *>(widget(i));
        if(tw && (tw->pluginName() == pw->pluginName())){
            int tabIndex = tw->property(PLUGIN_INDEX_PROPERTY).toInt();
            if(index < tabIndex) index = tabIndex;
        }
    }

    ConnectorPluginInterface *newWidget = pw;
    if(newWidget){
        newWidget->setProperty(PLUGIN_INDEX_PROPERTY, index +1);
        int tabIndex = addTab((QWidget*)newWidget
                              , newWidget->pluginIcon()
                              , pluginTabname(newWidget)
                              );
        connectPluginWidget(newWidget);
        setTabToolTip(tabIndex, newWidget->pluginName());
        setCurrentWidget((QWidget*)newWidget);
        connect(this, &ConnectorPluginManager::languageChanged,
                this, [=](){
            int index = indexOf(newWidget);
            if (index >= 0) {
                setTabToolTip(index, pluginTabname(newWidget));
                setTabText(index, pluginTabname(newWidget));
            }
        });
        emit pluginAdded(pluginId(newWidget));
    }
}

void ConnectorPluginManager::connectPluginWidget(ConnectorPluginInterface* pw)
{
    if (pw) {
        connect(pw, &ConnectorPluginInterface::measurementDataReady,
                this, &ConnectorPluginManager::measurementDataReady);

        connect(pw, &ConnectorPluginInterface::connexionReady,
                this, &ConnectorPluginManager::connexionReady);
        connect(pw, &ConnectorPluginInterface::connexionLost,
                this, &ConnectorPluginManager::connexionLost);

        connect(pw, &ConnectorPluginInterface::started,
                this, &ConnectorPluginManager::onConnectorStarted);
        connect(pw, &ConnectorPluginInterface::stopped,
                this, &ConnectorPluginManager::onConnectorStopped);
        connect(pw, &ConnectorPluginInterface::paused,
                this, &ConnectorPluginManager::onConnectorPaused);

        connect(pw, &ConnectorPluginInterface::fileOpened,
                this, &ConnectorPluginManager::fileOpened);

        connect(pw, &ConnectorPluginInterface::error,
                this, &ConnectorPluginManager::error);
        connect(pw, &ConnectorPluginInterface::information,
                this, &ConnectorPluginManager::information);
        connect(pw, &ConnectorPluginInterface::warning,
                this, &ConnectorPluginManager::warning);
    }
}

void ConnectorPluginManager::tabToClose(int tabIndex)
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(tabIndex));
    if (cw) {
        if (cw->isConnected()) {
            QMessageBox::warning(this,
                                 QApplication::applicationName(),
                                 tr("Device is connected. Disconnected first!"));

            return;
        }

        if(currentIndex() == tabIndex) setCurrentIndex(0);

        removeTab(tabIndex);
        emit pluginRemoved(pluginId(cw));

        m_dictionary.remove(pluginId(cw));

        cw->stop();
        cw->disconnect(this);
        cw->deleteLater();
    }

}

int ConnectorPluginManager::connectedCount()
{
    int nbConnected = 0;
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw) {
            if (cw->isConnected()) {
                nbConnected++;
            }
        }
    }

    return nbConnected;
}

void ConnectorPluginManager::startAll()
{
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if(cw){
            cw->start();
        }
    }
}

void ConnectorPluginManager::pauseAll()
{
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if(cw){
            cw->pause();
        }
    }
}

void ConnectorPluginManager::stopAll()
{
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if(cw) {
            cw->stop();
        }
    }
}

void ConnectorPluginManager::start()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(currentWidget());
    if (cw) {
        cw->start();
    }
}

void ConnectorPluginManager::pause()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(currentWidget());
    if (cw) {
        cw->pause();
    }
}

void ConnectorPluginManager::stop()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(currentWidget());
    if (cw) {
        cw->stop();
    }
}

void ConnectorPluginManager::connexionReady()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(sender());
    if(cw){
        emit connectorConnected(pluginId(cw));
    }
}

void ConnectorPluginManager::connexionLost()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(sender());
    if(cw){
        emit connectorDisconnected(pluginId(cw));
    }
}

void ConnectorPluginManager::measurementDataReady(const PluginDataStructure &data)
{
    ConnectorPluginInterface *cw = qobject_cast<ConnectorPluginInterface *>(sender());
    if(cw){
        emit connectorDataReady(pluginId(cw), data);
    }

}

void ConnectorPluginManager::onConnectorStarted()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(sender());
    if(cw){
        emit connectorStarted(pluginId(cw));
    }
}

void ConnectorPluginManager::onConnectorStopped()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(sender());
    if(cw){
        emit connectorStopped(pluginId(cw));
    }
}

void ConnectorPluginManager::onConnectorPaused()
{
    ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(sender());
    if(cw){
        emit connectorPaused(pluginId(cw));
    }
}

QString ConnectorPluginManager::pluginTabname(GeneralPluginInterface* pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginName();
    prefix += " " + tab;
    return prefix;
}

QString ConnectorPluginManager::pluginId(GeneralPluginInterface *pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginClassName() + ":" + pw->pluginVersion().toString();

    prefix += tab;
    m_dictionary[prefix] = pw;
    return prefix;
}

QString ConnectorPluginManager::pluginTitle(GeneralPluginInterface *pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginName();

    prefix += tab;
    return prefix;
}

QVector<PluginInformation> ConnectorPluginManager::pluginInformation()
{
    QVector<PluginInformation> idList;
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw) idList << PluginInformation(pluginId(cw), [=](){ return pluginTitle(cw);}, cw->pluginIcon());
    }
    return idList;
}

ConnectorState ConnectorPluginManager::connectorState(const QString &id) const
{
    if(m_dictionary.contains(id)){
        ConnectorPluginInterface *cw = (ConnectorPluginInterface *)m_dictionary[id];
        if(cw) return ConnectorState(cw->isConnected(), cw->isStarted(), cw->isPaused());
    }
    return ConnectorState();
}

void ConnectorPluginManager::loadFile(const QString &filename)
{
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw && cw->loadFile(filename)){
            setCurrentIndex(i);
            cw->start();
            break;
        }
    }
}

void ConnectorPluginManager::saveState(QSettings &settings)
{
    settings.setValue("currentIndex", currentIndex());

    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw){
            settings.beginGroup(pluginId(cw));
            cw->saveState(settings);
            settings.endGroup();
        }
    }
}

void ConnectorPluginManager::restoreState(QSettings &settings)
{
    for (int i = 0; i < count(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw){
            settings.beginGroup(pluginId(cw));
            cw->restoreState(settings);
            settings.endGroup();
        }
    }

    if(count() > 0){
        setCurrentIndex(settings.value("currentIndex", 0).toInt());
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(currentWidget());
        if(cw) emit currentPluginChanged(pluginId(cw));
    }
}

void ConnectorPluginManager::loadTranslator(const QString &locale)
{
    for (int i = 0; i < m_plugins.size(); ++i) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(widget(i));
        if (cw) cw->loadTranslator(locale);
    }
}

void ConnectorPluginManager::tabsChanged(int tabIndex)
{
    Q_UNUSED(tabIndex);

    QTabWidget* tb = (QTabWidget*)QObject::sender();
    if (tb) {
        ConnectorPluginInterface* cw = qobject_cast<ConnectorPluginInterface*>(tb->currentWidget());
        if(cw) emit currentPluginChanged(pluginId(cw));
    }
}

