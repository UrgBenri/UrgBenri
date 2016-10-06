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

#include "FilterPluginManager.h"

#include <QVBoxLayout>
#include <QDebug>

#define PLUGIN_INDEX_PROPERTY "PluginIndex"

FilterPluginManager::FilterPluginManager(QWidget *parent)
    : QTabWidget(parent)
    , m_addMenu(new QMenu(this))
    , m_addButton(new QToolButton(this))
{

    setTabsClosable(true);

    initAddActions();

    connect(this, &FilterPluginManager::currentChanged,
            this, &FilterPluginManager::tabsChanged);
    connect(this, &FilterPluginManager::tabCloseRequested,
            this, &FilterPluginManager::tabToClose);
}

FilterPluginManager::~FilterPluginManager()
{

}

void FilterPluginManager::initAddActions()
{
    m_addButton->setIcon(QIcon(QStringLiteral(":/FilterPluginManager/add_icon")));
    m_addButton->setIconSize(QSize(24, 24));
    m_addButton->setMenu(m_addMenu);
    m_addButton->setPopupMode(QToolButton::InstantPopup);

    connect(this, &FilterPluginManager::languageChanged,
            this, [=](){
        m_addButton->setToolTip(tr("Add new Filter"));
    });
    m_addButton->setToolTip(tr("Add new Filter"));

    setCornerWidget(m_addButton, Qt::TopRightCorner);
}

void FilterPluginManager::loadPlugins(const QVector<FilterPluginInterface *> &filters, PluginManagerInterface *manager)
{
    m_plugins = filters;
    m_pluginManager = manager;
    initPluginTabs();
}

void FilterPluginManager::initPluginTabs()
{
    foreach (FilterPluginInterface *pw, m_plugins) {
        if(pw){
            QAction *action = new QAction(m_addMenu);
            action->setText(pw->pluginName());
            action->setIcon(pw->pluginIcon());
            m_addMenu->addAction(action);
            connect(action, &QAction::triggered, this, [=](){
                FilterPluginInterface *pi = qobject_cast<FilterPluginInterface*>(pw->createPlugin(this));
                if(pi){
                    pi->onLoad(m_pluginManager);
                    addPluginWidget(pi);
                }
            });
            connect(this, &FilterPluginManager::languageChanged,
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

void FilterPluginManager::addPluginWidget(FilterPluginInterface *pw)
{
    int index = 0;
    for(int i = 0; i < count(); ++i){
        FilterPluginInterface *tw = qobject_cast<FilterPluginInterface *>(widget(i));
        if(tw && (tw->pluginName() == pw->pluginName())){
            int tabIndex = tw->property(PLUGIN_INDEX_PROPERTY).toInt();
            if(index < tabIndex) index = tabIndex;
        }
    }

    FilterPluginInterface *newWidget = pw;
    if(newWidget){
        newWidget->updateConnectorsList(m_sources);
        newWidget->setProperty(PLUGIN_INDEX_PROPERTY, index +1);
        int tabIndex = addTab((QWidget*)newWidget
                              , newWidget->pluginIcon()
                              , pluginTabname(newWidget)
                              );
        connectPluginWidget(newWidget);
        setTabToolTip(tabIndex, newWidget->pluginName());
        setCurrentWidget((QWidget*)newWidget);
        connect(this, &FilterPluginManager::languageChanged,
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

void FilterPluginManager::connectPluginWidget(FilterPluginInterface* pi)
{
    if (pi) {
        connect(pi, &FilterPluginInterface::measurementDataReady,
                this, &FilterPluginManager::measurementDataReady);

        connect(pi, &FilterPluginInterface::error,
                this, &FilterPluginManager::error);
        connect(pi, &FilterPluginInterface::information,
                this, &FilterPluginManager::information);
        connect(pi, &FilterPluginInterface::warning,
                this, &FilterPluginManager::warning);
    }
}

QString FilterPluginManager::pluginTabname(GeneralPluginInterface* pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginName();
    prefix += " " + tab;
    return prefix;
}

QString FilterPluginManager::pluginId(GeneralPluginInterface *pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginClassName() + ":" + pw->pluginVersion().toString();

    prefix += tab;
    m_dictionary[prefix] = pw;
    return prefix;
}

QString FilterPluginManager::pluginTitle(GeneralPluginInterface *pw)
{
    QString tab = QString::number(pw->property(PLUGIN_INDEX_PROPERTY).toInt());
    QString prefix = pw->pluginName();

    prefix += tab;
    return prefix;
}

void FilterPluginManager::saveState(QSettings &settings)
{
    settings.setValue("currentIndex", currentIndex());

    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (cw){
            settings.beginGroup(pluginId(cw));
            cw->saveState(settings);
            settings.endGroup();
        }
    }
}

void FilterPluginManager::restoreState(QSettings &settings)
{
    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (cw){
            settings.beginGroup(pluginId(cw));
            cw->restoreState(settings);
            settings.endGroup();
        }
    }

    if(count() > 0){
        setCurrentIndex(settings.value("currentIndex", 0).toInt());
        FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(currentWidget());
        if(cw) emit currentPluginChanged(pluginId(cw));
    }
}

void FilterPluginManager::loadTranslator(const QString &locale)
{
    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* fw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (fw){
            fw->loadTranslator(locale);
        }
    }
}

void FilterPluginManager::updateConnectorsList(const QVector<PluginInformation> &infos)
{
    m_sources = infos;
    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* fw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (fw) fw->updateConnectorsList(infos);
    }
}


void FilterPluginManager::onConnectorDataReady(const QString &id, const PluginDataStructure &data)
{
    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* fw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (fw) fw->addMeasurementData(id, data);
    }
}

void FilterPluginManager::measurementDataReady(const PluginDataStructure &data)
{
    FilterPluginInterface *fw = qobject_cast<FilterPluginInterface *>(sender());
    if(fw){
        emit connectorDataReady(pluginId(fw), data);
    }
}

QVector<PluginInformation> FilterPluginManager::pluginInformation()
{
    QVector<PluginInformation> idList;
    for (int i = 0; i < count(); ++i) {
        FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(widget(i));
        if (cw) idList << PluginInformation(pluginId(cw), [=](){ return pluginTitle(cw);}, cw->pluginIcon());
    }
    return idList;
}

void FilterPluginManager::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange: {
            emit languageChanged();
        }break;
        default: {
        }
    }
}

void FilterPluginManager::tabToClose(int tabIndex)
{
    FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(widget(tabIndex));
    if (cw) {
        if(currentIndex() == tabIndex) setCurrentIndex(0);

        removeTab(tabIndex);
        emit pluginRemoved(pluginId(cw));

        m_dictionary.remove(pluginId(cw));

        cw->disconnect(this);
        cw->deleteLater();
    }
}

void FilterPluginManager::tabsChanged(int tabIndex)
{
    Q_UNUSED(tabIndex);

    QTabWidget* tb = (QTabWidget*)QObject::sender();
    if (tb) {
        FilterPluginInterface* cw = qobject_cast<FilterPluginInterface*>(tb->currentWidget());
        if(cw) emit currentPluginChanged(pluginId(cw));
    }
}

