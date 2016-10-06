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

#ifndef LOGCONSOLEWIDGET_H
#define LOGCONSOLEWIDGET_H

#include <QWidget>

namespace Ui {
class LogConsoleWidget;
}

class LogConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogConsoleWidget(QWidget *parent = 0);
    virtual ~LogConsoleWidget();

    int maximumLineCount() const;
    void setMaximumLineCount(int maximumLineCount);

protected:
    void changeEvent(QEvent *e);

public slots:
    void clearButttonClicked();
    void saveButttonClicked();
    void addError(const QString &title, const QString &message);
    void addInformation(const QString &title, const QString &message);
    void addWarning(const QString &title, const QString &message);

private:
    Ui::LogConsoleWidget *ui;
    int m_maximumLineCount;
};

#endif // LOGCONSOLEWIDGET_H

