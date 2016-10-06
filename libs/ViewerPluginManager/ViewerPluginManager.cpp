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

#include "ViewerPluginManager.h"
#include "ui_ViewerPluginManager.h"

#include <QDebug>
#include <QTabBar>
#include <QDir>
#include <QFile>
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include "PluginManager.h"

ViewerPluginManager::ViewerPluginManager(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ViewerPluginManager)
    , m_rightCornelLabel(new QLabel(this))
    , m_leftCornelLabel(new QLabel(this))
    , m_receptionTimeAvg(40, 25)
{
    ui->setupUi(this);


    ui->viewers->setStyleSheet("QTabWidget::tab-bar {"
                               "alignment: center;"
                               "}");


    addCornerLabel(m_rightCornelLabel, Qt::BottomRightCorner);
    addCornerLabel(m_leftCornelLabel, Qt::BottomLeftCorner);

    setLeftLabelVisible(true);

    m_receptionTimeAvg.setAverageValue(25);

    setLeftLabelText(tr("Rate: %1ms | %2fps").arg(0).arg(0));
}

ViewerPluginManager::~ViewerPluginManager()
{
    delete ui;
}

void ViewerPluginManager::loadViewers(const QVector<ViewerPluginInterface *> &viewers, PluginManagerInterface *manager)
{
    foreach (ViewerPluginInterface *plugin, viewers){
        ViewerPluginInterface *viewer = qobject_cast<ViewerPluginInterface *>(plugin->createPlugin(this));
        if(viewer){
            viewer->onLoad(manager);
            connect(viewer, SIGNAL(error(const QString&,const QString&)),
                    this, SIGNAL(error(const QString&,const QString&)));
            connect(viewer, SIGNAL(information(const QString&,const QString&)),
                    this, SIGNAL(information(const QString&,const QString&)));
            connect(viewer, SIGNAL(warning(const QString&,const QString&)),
                    this, SIGNAL(warning(const QString&,const QString&)));
            m_viewers << viewer;
        }
    }

    ui->noViewersLabel->setVisible(m_viewers.size() == 0);
    ui->viewers->setVisible(m_viewers.size() > 0);

    addViewers();

    connectViewers();
}

void ViewerPluginManager::addCornerLabel(QLabel *label, Qt::Corner corner)
{
    label->setFrameShape(QFrame::StyledPanel);
    label->setFrameShadow(QFrame::Raised);
    label->setMargin(3);
    label->setVisible(false);
    ui->viewers->setCornerWidget(label, corner);
}

void ViewerPluginManager::connectViewers()
{
    foreach (ViewerPluginInterface *viewer1, m_viewers) {
        foreach (ViewerPluginInterface *viewer2, m_viewers) {
            if(viewer1 != viewer2){
                connect(viewer1, &ViewerPluginInterface::selectedStepChanged,
                        viewer2, &ViewerPluginInterface::setSelectedStep);
                connect(viewer2, &ViewerPluginInterface::selectedStepChanged,
                        viewer1, &ViewerPluginInterface::setSelectedStep);
            }
        }

        connect(viewer1, &ViewerPluginInterface::selectedStepChanged,
                this, &ViewerPluginManager::selectedStepChanged);
    }
}

void ViewerPluginManager::addViewers()
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        int tabIndex = ui->viewers->addTab(viewer, viewer->pluginIcon(), viewer->pluginName());
        ui->viewers->tabBar()->setTabButton(tabIndex, QTabBar::RightSide, viewer->getActiveWidget());
        connect(viewer, &ViewerPluginInterface::error,
                this, &ViewerPluginManager::error);
        connect(viewer, &ViewerPluginInterface::information,
                this, &ViewerPluginManager::information);
        connect(viewer, &ViewerPluginInterface::warning,
                this, &ViewerPluginManager::warning);
    }
}

void ViewerPluginManager::updateTabLabel()
{
    for(int i =0; i < ui->viewers->count(); ++i){
        ViewerPluginInterface *viewer = qobject_cast<ViewerPluginInterface *>(ui->viewers->widget(i));
        if(viewer){
            ui->viewers->setTabText(i, viewer->pluginName());
        }
    }
}

void ViewerPluginManager::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if(ui) ui->retranslateUi(this);
        updateTabLabel();
        break;
    default:
        break;
    }
}

void ViewerPluginManager::refresh()
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer->isActive()){
            viewer->refresh();
        }
    }
}

void ViewerPluginManager::clear()
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer->isActive()){
            viewer->clear();
        }
    }
    setLeftLabelText(tr("Rate: %1ms | %2fps").arg(0).arg(0));
}

void ViewerPluginManager::addMeasurementData(const QString &id
                                             , const PluginDataStructure &data)
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer->isActive()){
            viewer->addMeasurementData(id, data);
        }
    }
    m_receptionTimeAvg.push_back(m_receptionTimer.restart());
    if (m_receptionTimeAvg.average() > 0) {
        setLeftLabelText(tr("Rate: %1ms | %2fps")
                         .arg(m_receptionTimeAvg.average())
                         .arg(1000 / (m_receptionTimeAvg.average()))
                         );
    }
}

void ViewerPluginManager::setRightLabelText(const QString &text)
{
    m_rightCornelLabel->setText(text);
}

void ViewerPluginManager::setLeftLabelText(const QString &text)
{
    m_leftCornelLabel->setText(text);
}

void ViewerPluginManager::setRightLabelVisible(bool visible)
{
    m_rightCornelLabel->setVisible(visible);
}

void ViewerPluginManager::setLeftLabelVisible(bool visible)
{
    m_leftCornelLabel->setVisible(visible);
}

void ViewerPluginManager::saveState(QSettings &settings)
{
    settings.setValue("currentIndex", ui->viewers->currentIndex());
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer){
            settings.beginGroup(viewer->metaObject()->className());
            settings.setValue("active", viewer->isActive());
            viewer->saveState(settings);
            settings.endGroup();
        }
    }
}

void ViewerPluginManager::restoreState(QSettings &settings)
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer){
            settings.beginGroup(viewer->metaObject()->className());
            viewer->setActive(settings.value("active", true).toBool());
            viewer->restoreState(settings);
            settings.endGroup();
        }
    }
    ui->viewers->setCurrentIndex(settings.value("currentIndex", 0).toInt());
}

void ViewerPluginManager::loadTranslator(const QString &locale)
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer){
            viewer->loadTranslator(locale);
        }
    }
}

void ViewerPluginManager::setSelectedStep(int step)
{
    foreach (ViewerPluginInterface *viewer, m_viewers) {
        if(viewer->isActive()){
            viewer->setSelectedStep(step);
        }
    }
}

