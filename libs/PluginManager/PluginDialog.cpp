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

#include "PluginDialog.h"

#include <QGridLayout>
#include <QDir>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QProcess>

#include "PluginManager.h"

PluginDialog::PluginDialog(QWidget *parent)
    : QDialog(parent)
    , m_label(new QLabel)
    , m_tableView(new QTableView())
    , m_okButton(new QPushButton(tr("OK")))
    , m_addButton(new QPushButton(tr("Add Plugin")))
    , m_restartButton(new QPushButton(tr("Restart")))
    , m_manager(new PluginManager(this))
{
    setObjectName(QStringLiteral("PluginManagerDialog"));

    QSortFilterProxyModel *sorter = new QSortFilterProxyModel(this);
    sorter->setSourceModel(m_manager);
    m_tableView->setModel(sorter);
    m_tableView->setSortingEnabled(true);

    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->verticalHeader()->hide();
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setWordWrap(true);
    m_tableView->resizeColumnsToContents();

    setMinimumWidth(600);

    m_okButton->setDefault(true);

    connect(m_okButton, &QPushButton::clicked,
            this, &PluginDialog::close);

    connect(m_addButton, &QPushButton::clicked,
            this, &PluginDialog::addPlugin);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->addWidget(m_label, 0, 0, 1, 3);
    mainLayout->addWidget(m_tableView, 1, 0, 1, 3);
    mainLayout->addWidget(m_okButton, 2, 2);
    mainLayout->addWidget(m_restartButton, 2, 1);
    mainLayout->addWidget(m_addButton, 2, 0);
    setLayout(mainLayout);

    m_restartButton->setEnabled(false);
    connect(m_manager, &PluginManager::restartRequired,
            this, [=](){
       m_restartButton->setEnabled(true);
    });

    connect(m_restartButton, &QPushButton::clicked,
            this, [&](){
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    });

    setWindowTitle(tr("Plugin Information"));

    QString paths = tr("UrgBenri found the following plugins\n"
                       "looked in:") + "\n";
    foreach (const QString &path, m_manager->pluginPaths()) {
        paths += QDir::toNativeSeparators(path) + "\n";
    }
    m_label->setText(paths);
}

PluginDialog::~PluginDialog()
{

}

void PluginDialog::saveState(QSettings &settings)
{
    settings.beginGroup(m_manager->metaObject()->className());
    m_manager->saveState(settings);
    settings.endGroup();
}

void PluginDialog::restoreState(QSettings &settings)
{
    settings.beginGroup(m_manager->metaObject()->className());
    m_manager->restoreState(settings);
    settings.endGroup();
}

void PluginDialog::installPlugin(const QString &filename)
{
    QFileInfo fi(filename);
    QString pluginPath = m_manager->pluginPaths()[1];
    QDir().mkpath(pluginPath);
    if(fi.exists()){
        QString distFilename = QString("%1/%2")
                .arg(pluginPath)
                .arg(fi.fileName());
        if(QFile::copy(filename, distFilename)){
            m_manager->loadPlugin(distFilename);
            QMessageBox::information(this, QApplication::applicationName(),
                                     tr("The plugin is installed.") + "\n"+
                                     tr("The plugin will be loaded after restarting UrgBenriPlus"));
        }
        else{
            QMessageBox::critical(this, QApplication::applicationName(),
                                  tr("The plugin could not be installed."));
        }
    }
    else{
        QMessageBox::critical(this, QApplication::applicationName(),
                               tr("The plugin does not exist."));
    }
}

void PluginDialog::retranslateUi()
{
    m_okButton->setText(tr("OK"));
    m_addButton->setText(tr("Add Plugin"));
    m_restartButton->setText(tr("Restart"));

    setWindowTitle(tr("Plugin Information"));

    QString paths = tr("UrgBenri found the following plugins\n"
                       "looked in:") + "\n";
    foreach (const QString &path, m_manager->pluginPaths()) {
        paths += QDir::toNativeSeparators(path) + "\n";
    }
    m_label->setText(paths);
}

void PluginDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: {
        retranslateUi();
    }break;
    default: {
    }
    }
}

void PluginDialog::addPlugin()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Select Plugin file"),
                QDir::currentPath(),
                tr("Plugin file") +
            #ifdef Q_OS_WIN
                " (*.dll)"
            #endif
            #ifdef Q_OS_LINUX
                " (*.so)"
            #endif
            #ifdef Q_OS_OSX
                " (*.dylib)"
            #endif
                );

    if (!filename.isNull()) {
        installPlugin(filename);
    }
}

