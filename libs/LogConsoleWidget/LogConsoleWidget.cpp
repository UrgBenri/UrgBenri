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

#include "LogConsoleWidget.h"
#include "ui_LogConsoleWidget.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>

LogConsoleWidget::LogConsoleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogConsoleWidget)
    , m_maximumLineCount(5000)
{
    ui->setupUi(this);
    ui->logConsole->setCenterOnScroll(true);
    ui->logConsole->setReadOnly(true);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &LogConsoleWidget::clearButttonClicked);
    connect(ui->saveButton, &QPushButton::clicked,
            this, &LogConsoleWidget::saveButttonClicked);
}

LogConsoleWidget::~LogConsoleWidget()
{
    delete ui;
}
int LogConsoleWidget::maximumLineCount() const
{
    return m_maximumLineCount;
}

void LogConsoleWidget::setMaximumLineCount(int maximumLineCount)
{
    m_maximumLineCount = maximumLineCount;
    ui->logConsole->setMaximumBlockCount(m_maximumLineCount);
}

void LogConsoleWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: {
        if (ui) ui->retranslateUi(this);
    }break;
    default: {
    }
    }
}

void LogConsoleWidget::clearButttonClicked()
{
    ui->logConsole->clear();
}

void LogConsoleWidget::saveButttonClicked()
{
    QString defaulName = QString("/log.txt");
    QString filename = QFileDialog::getSaveFileName(
                this,
                tr("Save log information"),
                QDir::currentPath() + defaulName,
                tr("Text file (*.txt)"));
    if (!filename.isNull()) {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            stream << ui->logConsole->toPlainText();

            file.close();
            QMessageBox::information(this, QApplication::applicationName(),
                             tr("Log information were saved successfully."));
        }
        else {
            QMessageBox::critical(this, QApplication::applicationName(),
                       tr("Log file could not be created."));
        }
    }
}

void LogConsoleWidget::addError(const QString &title, const QString &message)
{
    ui->logConsole->appendHtml(QString("<font color=red>[%1] %2</font>")
                               .arg(title)
                               .arg(message));
}

void LogConsoleWidget::addInformation(const QString &title, const QString &message)
{
    ui->logConsole->appendHtml(QString("<font color=blue>[%1] %2</font>")
                               .arg(title)
                               .arg(message));
}

void LogConsoleWidget::addWarning(const QString &title, const QString &message)
{
    ui->logConsole->appendHtml(QString("<font color=orange>[%1] %2</font>")
                               .arg(title)
                               .arg(message));
}

