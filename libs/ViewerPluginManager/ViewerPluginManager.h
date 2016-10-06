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

#ifndef ViewerWidget_H
#define ViewerWidget_H

#include <QWidget>
#include <QLabel>
#include <QSettings>
#include <QTime>

#include "ViewerPluginInterface.h"
#include "MovingAverage.h"

namespace Ui
{
class ViewerPluginManager;
}

class ViewerPluginManager : public QWidget
{
    Q_OBJECT

public:
    explicit ViewerPluginManager(QWidget* parent = 0);
    virtual ~ViewerPluginManager();


    void setRightLabelText(const QString &text);
    void setLeftLabelText(const QString &text);

    void loadViewers(const QVector<ViewerPluginInterface *> &viewers, PluginManagerInterface *manager);

    void setRightLabelVisible(bool visible);
    void setLeftLabelVisible(bool visible);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);

protected:
    void changeEvent(QEvent* e);

signals:
    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);
    void selectedStepChanged(int step);


public slots:
    void addMeasurementData(const QString &id
                            , const PluginDataStructure &data);
    void refresh();
    void clear();
    void setSelectedStep(int step);

private:
    Ui::ViewerPluginManager* ui;
    QVector<ViewerPluginInterface *> m_viewers;
    QLabel *m_rightCornelLabel;
    QLabel *m_leftCornelLabel;
    MovingAverage<int> m_receptionTimeAvg;
    QTime m_receptionTimer;

    void addViewers();
    void connectViewers();
    void updateTabLabel();
    void addCornerLabel(QLabel *label, Qt::Corner corner = Qt::TopRightCorner);
};

#endif // ViewerWidget_H

