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

#include "QuadSplitter.h"

#include <QVBoxLayout>

class HackedSplitter: public QSplitter
{
public:
    void moveSplitter(int pos, int index){
        moveSplitter(pos, index);
    }
};

QuadSplitter::QuadSplitter(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    m_mainSplitter = new QSplitter(Qt::Vertical, this);
    m_mainSplitter->setChildrenCollapsible(false);
    m_mainSplitter->setStyleSheet("QSplitter::handle {background: #595959; border: none;}");
    m_mainSplitter->setVisible(false);

    m_noViewerLabel = new QLabel(this);
    m_noViewerLabel->setText(tr("No viewer"));


    layout->addWidget(m_mainSplitter);
    layout->addWidget(m_noViewerLabel);
    setLayout(layout);
}

QuadSplitter::~QuadSplitter()
{

}

void QuadSplitter::addWidget(QWidget *widget)
{
    if((m_widgets.size() % 2) == 0){
        QSplitter *row = new QSplitter(Qt::Horizontal, this);
        row->setChildrenCollapsible(false);
        row->setStyleSheet(m_mainSplitter->styleSheet());
        m_mainSplitter->addWidget(row);
        row->addWidget(widget);
    }
    else{
        QSplitter *row = (QSplitter *)m_mainSplitter->widget(m_mainSplitter->count() -1);
        row->addWidget(widget);
    }

    m_widgets.append(widget);

    m_noViewerLabel->setVisible(m_widgets.size() == 0);
    m_mainSplitter->setVisible(m_widgets.size() > 0);
}

QWidget *QuadSplitter::widget(int index)
{
    return m_widgets.at(index);
}

