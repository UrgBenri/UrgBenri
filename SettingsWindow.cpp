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

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDir>

const QString LogHeader = "ApplicationSetting";

SettingsWindow::SettingsWindow(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat)
{
    ui->setupUi(this);
    setupConnections();
}

SettingsWindow::~SettingsWindow()
{
    qDebug() << "SettingsWindow::~SettingsWindow";
    delete ui;
}

void SettingsWindow::setupConnections()
{
    connect(ui->closeButton, &QAbstractButton::clicked,
            this, &QWidget::close);
    connect(ui->saveButton, &QAbstractButton::clicked,
            this, &SettingsWindow::saveButtonClicked);
    connect(ui->resetButton, &QAbstractButton::clicked,
            this, &SettingsWindow::resetButtonClicked);
}

bool SettingsWindow::getSetting(const QString &key, bool defaultValue)
{
    QSettings mySettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat);
    return mySettings.value(key, defaultValue).toBool();
}

int SettingsWindow::getSetting(const QString &key, int defaultValue)
{
    QSettings mySettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat);
    return mySettings.value(key, defaultValue).toInt();
}

void SettingsWindow::saveState(QSettings &settings)
{
    settings.setValue("minimumFreeSpace", ui->minDiskSpace->value());
}

void SettingsWindow::restoreState(QSettings &settings)
{
    ui->minDiskSpace->setValue(settings.value("minimumFreeSpace", 500).toInt());
}

void SettingsWindow::saveButtonClicked()
{
    emit information(LogHeader,
                         tr("Settings saved sucessfuly."));
    close();
}

void SettingsWindow::resetButtonClicked()
{
    QDir().remove(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini");
}

void SettingsWindow::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        if(ui) ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

