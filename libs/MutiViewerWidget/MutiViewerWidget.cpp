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

#include "MutiViewerWidget.h"
#include "ui_MutiViewerWidget.h"

#include <QDebug>
#include <QTimer>

#define MAIN_VIEWER_INDEX 0

MutiViewerWidget::MutiViewerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MutiViewerWidget)
{
    ui->setupUi(this);

    connect(ui->fullscreen, &QToolButton::toggled,
            this, &MutiViewerWidget::fullScreenToggled);

    m_viewerSplitter = ui->widgets;

    m_viewers.push_back(new ViewerPluginManager(m_viewerSplitter));
    m_selectors.push_back(ui->viewer1);
    m_viewers.push_back(new ViewerPluginManager(m_viewerSplitter));
    m_selectors.push_back(ui->viewer2);
    m_viewers.push_back(new ViewerPluginManager(m_viewerSplitter));
    m_selectors.push_back(ui->viewer3);
    m_viewers.push_back(new ViewerPluginManager(m_viewerSplitter));
    m_selectors.push_back(ui->viewer4);

    addViewers();

    m_souces.clear();
    updateConnectorsList(m_souces);

    connectViewers();

    connectSelectors();

    bool isFirst = false;
    foreach (ViewerPluginManager *viewer, m_viewers) {
        viewer->setRightLabelVisible(true);
        if(!isFirst){
            isFirst = true;
            viewer->hide();
        }
    }

    m_colors.push_back(QColor(Qt::yellow).lighter(180));
    m_colors.push_back(QColor(Qt::green).lighter(180));
    m_colors.push_back(QColor(Qt::blue).lighter(180));
    m_colors.push_back(QColor(Qt::red).lighter(180));

    updateColors();
    checkVewerState();
}

void MutiViewerWidget::connectViewers()
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        connect(viewer, &ViewerPluginManager::selectedStepChanged,
                this, &MutiViewerWidget::selectedStepChanged, Qt::UniqueConnection);
    }
}

void MutiViewerWidget::connectSelectors()
{
    foreach (QComboBox *box, m_selectors) {
        connect(box, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &MutiViewerWidget::updateViewer, Qt::UniqueConnection);
    }
}

void MutiViewerWidget::updateColors()
{
    for(int i = 0; i < m_colors.size(); ++i) {
        QColor color = m_colors[i];
        setBackgroundColor(m_viewers[i], color);
        setBackgroundColor(m_selectors[i]->parentWidget(), color);
        m_selectors[i]->setFrame(true);
    }
}

void MutiViewerWidget::updateViewer(int index)
{
    QComboBox *box = qobject_cast<QComboBox *>(sender());
    if(box){
        QString sourceId = box->itemData(index, Qt::UserRole).toString();
        ViewerPluginManager *viewer = m_viewers.at(m_selectors.indexOf(box));
        viewer->setVisible(sourceId != "None");
        viewer->setProperty("source", sourceId);
        viewer->setRightLabelText(tr("Source: ") + box->itemText(index));
        viewer->clear();
    }
    checkVewerState();
}

void MutiViewerWidget::updateViewersSource()
{
    for(int i = 0 ; i < m_viewers.size(); ++i){
        ViewerPluginManager *viewer = m_viewers[i];
        QComboBox *box = m_selectors[i];
        if(viewer && box){
            viewer->setProperty("source", box->currentData(Qt::UserRole).toString());
            viewer->setRightLabelText(tr("Source: ") + box->currentText());
        }
    }
}

void MutiViewerWidget::checkVewerState()
{
    bool allHidden = true;
    foreach (QComboBox *selector, m_selectors) {
        if(selector->count() > 1){
            allHidden = false;
            break;
        }
    }

    m_viewerSplitter->setVisible(!allHidden);
    ui->noConnectorsLabel->setVisible(allHidden);
}

void MutiViewerWidget::selectedStepChanged(int step)
{
    ViewerPluginManager *sourceViewer = qobject_cast<ViewerPluginManager *>(sender());
    if(sourceViewer){
        QString id = sourceViewer->property("source").toString();
        foreach (ViewerPluginManager *viewer, m_viewers) {
            if((sourceViewer != viewer) && (viewer->property("source").toString() == id)){
                viewer->setSelectedStep(step);
            }
        }
    }
}

void MutiViewerWidget::addViewers()
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        viewer->hide();
        m_viewerSplitter->addWidget(viewer);
        connect(viewer, &ViewerPluginManager::error,
                this, &MutiViewerWidget::error);
        connect(viewer, &ViewerPluginManager::information,
                this, &MutiViewerWidget::information);
        connect(viewer, &ViewerPluginManager::warning,
                this, &MutiViewerWidget::warning);
    }
}

MutiViewerWidget::~MutiViewerWidget()
{
    delete ui;
}

void MutiViewerWidget::setBackgroundColor(QWidget *widget, const QColor &color)
{
    if(widget){
        QPalette Pal = widget->palette();
        Pal.setColor(QPalette::Background, color);
        Pal.setColor(QPalette::Base, color);
        widget->setAutoFillBackground(true);
        widget->setPalette(Pal);
    }
}

void MutiViewerWidget::addMeasurementData(const QString &id
                                           , const PluginDataStructure &data)
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        if(viewer->property("source").toString() == id){
            viewer->addMeasurementData(id, data);
        }
    }
}

void MutiViewerWidget::updateConnectorsList(const QVector<PluginInformation> &list)
{
    m_souces = list;
    foreach (QComboBox *box, m_selectors) {
        updateSourceViewer(box);
    }

    if(m_selectors[MAIN_VIEWER_INDEX]->count() > 1){
        m_selectors[MAIN_VIEWER_INDEX]->removeItem(0);
    }
}

void MutiViewerWidget::setMainViewerSource(const QString source)
{
    for(int i = 0; i < m_selectors[MAIN_VIEWER_INDEX]->count(); ++i){
        QString index = m_selectors[MAIN_VIEWER_INDEX]->itemData(i, Qt::UserRole).toString();
        if(source == index){
            m_selectors[MAIN_VIEWER_INDEX]->setCurrentIndex(i);
            break;
        }
    }
}

int MutiViewerWidget::visibleViewerCount()
{
    int count = 0;
    foreach (ViewerPluginManager *viewer, m_viewers) {
        if(viewer && viewer->isVisible()) count++;
    }

    return count;
}

void MutiViewerWidget::saveState(QSettings &settings)
{
    for(int i = 0; i < m_viewers.size(); ++i){
        ViewerPluginManager *viewer = m_viewers[i];
        if(viewer){
            settings.beginGroup(QString("%1_%2").arg(viewer->metaObject()->className()).arg(i));
            viewer->saveState(settings);
            settings.endGroup();
        }
        QComboBox *box = m_selectors[i];
        if(box){
            settings.beginGroup(QString("%1_%2").arg("ViewerSelector").arg(i));
            settings.setValue("currentIndex", box->currentIndex());
            settings.endGroup();
        }
    }
}

void MutiViewerWidget::restoreState(QSettings &settings)
{
    for(int i = 0; i < m_viewers.size(); ++i){
        ViewerPluginManager *viewer = m_viewers[i];
        if(viewer){
            settings.beginGroup(QString("%1_%2").arg(viewer->metaObject()->className()).arg(i));
            viewer->restoreState(settings);
            settings.endGroup();
        }
        QComboBox *box = m_selectors[i];
        if(box){
            settings.beginGroup(QString("%1_%2").arg("ViewerSelector").arg(i));
            box->setCurrentIndex(settings.value("currentIndex", 0).toInt());
            settings.endGroup();
        }
    }
}

void MutiViewerWidget::loadPlugins(const QVector<ViewerPluginInterface *> &viewers, PluginManagerInterface *manager)
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        if(viewer){
            viewer->loadViewers(viewers, manager);
        }
    }
}

void MutiViewerWidget::loadTranslator(const QString &locale)
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        if(viewer){
            viewer->loadTranslator(locale);
        }
    }
}

void MutiViewerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if(ui) ui->retranslateUi(this);
        foreach (QComboBox *box, m_selectors) {
            retranslateComboBox(box);
        }
        updateViewersSource();
        break;
    default:
        break;
    }
}

void MutiViewerWidget::clear(const QString &id)
{
    foreach (ViewerPluginManager *viewer, m_viewers) {
        if(viewer->property("source").toString() == id){
            viewer->clear();
        }
    }
}

void MutiViewerWidget::setFullScreenChecked(bool state)
{
    ui->fullscreen->setChecked(state);
}

void MutiViewerWidget::updateSourceViewer(QComboBox *box)
{
    if(box){
        QString index = box->itemData(box->currentIndex(), Qt::UserRole).toString();
        box->blockSignals(true);
        box->clear();
        box->addItem(QIcon(":/icons/none_icon"), tr("None"), "None");
        foreach (const PluginInformation &item, m_souces) {
            box->addItem(item.icon(), item.title(), item.id());
        }
        box->blockSignals(false);

        int find = -1;
        for(int i = 0; i < box->count(); ++i){
            if(box->itemData(i, Qt::UserRole).toString() == index){
                find = i;
                break;
            }
        }
        if(find < 0){
            box->setCurrentIndex(0);
        }else{
            box->setCurrentIndex(find);
        }
    }
}

void MutiViewerWidget::retranslateComboBox(QComboBox *box)
{
    if(box && (box->count() >= m_souces.size())){
        bool noneExists = box->count() > m_souces.size();
        if(noneExists) box->setItemText(0, tr("None"));
        int startCount = box->count() - m_souces.size();
        for(int i = startCount; i < box->count(); ++i){
            box->setItemText(i, m_souces[i - startCount].title());
        }
    }
}

