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

#ifndef MUTIVIEWERWIDGET_H
#define MUTIVIEWERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QIcon>
#include "QuadSplitter.h"
#include "ViewerPluginManager.h"
#include "PluginInformation.h"

namespace Ui {
class MutiViewerWidget;
}

class MutiViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MutiViewerWidget(QWidget *parent = 0);
    virtual ~MutiViewerWidget();
    typedef enum{
       SERIAL_TYPE = 0,
        ETHERNET_TYPE = 1,
        FILE_TYPE = 2,
    }SourceType;


    void updateConnectorsList(const QVector<PluginInformation> &list);

    void setMainViewerSource(const QString source);

    int visibleViewerCount();

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadPlugins(const QVector<ViewerPluginInterface *> &viewers, PluginManagerInterface *manager);
    void loadTranslator(const QString &locale);

signals:
    void fullScreenToggled(bool state);

    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);

protected:
    void changeEvent(QEvent* e);

public slots:
    void addMeasurementData(const QString &id
                           , const PluginDataStructure &data);
    void clear(const QString &id);
    void setFullScreenChecked(bool state);

private:
    Ui::MutiViewerWidget *ui;

    QVector<ViewerPluginManager *> m_viewers;
    QVector<QComboBox *> m_selectors;
    QVector<QColor> m_colors;
    QuadSplitter *m_viewerSplitter;
    QVector<PluginInformation> m_souces;

    void updateSourceViewer(QComboBox *box);

    void setBackgroundColor(QWidget *widget, const QColor &color);
    void addViewers();
    void updateColors();
    void connectSelectors();
    void connectViewers();
    void checkVewerState();
    void retranslateComboBox(QComboBox *box);

private slots:
    void updateViewersSource();
    void updateViewer(int index);
    void selectedStepChanged(int step);
};

#endif // MUTIVIEWERWIDGET_H

