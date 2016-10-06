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

#include "mainwindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QToolButton>
#include <QFileDialog>
#include <QShortcut>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>
#include <QMimeData>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QProcess>
#include <QProgressBar>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , translator()
    , ui(new Ui::MainWindow)
    , m_pauseMode(false)
    , m_pluginDialog(new PluginDialog(this))
    , m_settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/Settings.ini", QSettings::IniFormat)
{
    ui->setupUi(this);
    qDebug() << "UI initialised";

    m_viewerManager = ui->viewerWidget;
    m_connectorManager = ui->deviceBox;
    m_filterManager = ui->filtersManager;

    m_settings.beginGroup(m_pluginDialog->metaObject()->className());
    m_pluginDialog->restoreState(m_settings);
    m_settings.endGroup();

    m_connectorManager->loadPlugins(m_pluginDialog->getPluginManager()->getPlugins<ConnectorPluginInterface>()
                                    , m_pluginDialog->getPluginManager());
    m_filterManager->loadPlugins(m_pluginDialog->getPluginManager()->getPlugins<FilterPluginInterface>()
                                 , m_pluginDialog->getPluginManager());
    m_viewerManager->loadPlugins(m_pluginDialog->getPluginManager()->getPlugins<ViewerPluginInterface>()
                                 , m_pluginDialog->getPluginManager());

    m_tools = m_pluginDialog->getPluginManager()->getPlugins<ToolPluginInterface>();
    m_systems = m_pluginDialog->getPluginManager()->getPlugins<SystemPluginInterface>();
    foreach (SystemPluginInterface *spi, m_systems) {
         spi->initialize(this);
    }

    connect(ui->actionQuit, &QAction::triggered,
            this, &MainWindow::close);

    connect(ui->logConsoleDock, &QDockWidget::visibilityChanged,
            ui->actionLog_Console, &QAction::setChecked);
    ui->actionLog_Console->setChecked(ui->logConsoleDock->isVisible());
    connect(ui->actionLog_Console, &QAction::toggled,
            ui->logConsoleDock, &QDockWidget::setVisible);


    connect(m_connectorManager, &ConnectorPluginManager::error,
            ui->logConsole, &LogConsoleWidget::addError);
    connect(m_connectorManager, &ConnectorPluginManager::information,
            ui->logConsole, &LogConsoleWidget::addInformation);
    connect(m_connectorManager, &ConnectorPluginManager::warning,
            ui->logConsole, &LogConsoleWidget::addWarning);

    connect(m_filterManager, &FilterPluginManager::error,
            ui->logConsole, &LogConsoleWidget::addError);
    connect(m_filterManager, &FilterPluginManager::information,
            ui->logConsole, &LogConsoleWidget::addInformation);
    connect(m_filterManager, &FilterPluginManager::warning,
            ui->logConsole, &LogConsoleWidget::addWarning);

    connect(m_viewerManager, &MutiViewerWidget::error,
            ui->logConsole, &LogConsoleWidget::addError);
    connect(m_viewerManager, &MutiViewerWidget::information,
            ui->logConsole, &LogConsoleWidget::addInformation);
    connect(m_viewerManager, &MutiViewerWidget::warning,
            ui->logConsole, &LogConsoleWidget::addWarning);
    ui->logConsoleDock->close();

//    connect(&m_settingsWindow, &SettingsWindow::error,
//            ui->logConsole, &LogConsoleWidget::addError);
//    connect(&m_settingsWindow, &SettingsWindow::information,
//            ui->logConsole, &LogConsoleWidget::addInformation);
//    connect(&m_settingsWindow, &SettingsWindow::warning,
//            ui->logConsole, &LogConsoleWidget::addWarning);

    setAcceptDrops(true);

    setWindowTitle(QApplication::applicationName() + QString(" V") + QApplication::applicationVersion() +
                   QString(" By ") + QApplication::organizationName()
                   );

    updatePlayControls(false, false, false);

    setupMenus();
    initLanguage();

    setupConnections();

    updateViewerSources();

    initPlayControls();

    setupShortcuts();

    restoreState();

    ui->actionFrancais->setVisible(false);

    ui->noConnectorLabel->setVisible(m_connectorManager->count() == 0);
    m_connectorManager->setVisible(m_connectorManager->count() > 0);

    QTimer::singleShot(0, this, SLOT(checkForLoadFile()));

    //    deviceTabsChanged(m_connectorManager->currentConnector());

    if(m_filterManager->pluginCount() == 0){
        ui->filtersDock->toggleViewAction()->setVisible(false);
        ui->filtersDock->close();
    }
}

void MainWindow::notifyError(const QString &header, const QString &message)
{
    Q_UNUSED(header);
    statusBar()->showMessage(tr("Error") + ": " + message);
}

void MainWindow::notifyWarning(const QString &header, const QString &message)
{
    Q_UNUSED(header);
    statusBar()->showMessage(tr("Warning") + ": " + message);
}

void MainWindow::notifyInformation(const QString &header, const QString &message)
{
    Q_UNUSED(header);
    statusBar()->showMessage(tr("Information") + ": " + message);
}

void MainWindow::initPlayControls()
{
    connect(ui->playControls, &PlayControlsWidget::start, m_connectorManager, &ConnectorPluginManager::start);
    connect(ui->playControls, &PlayControlsWidget::stop, m_connectorManager, &ConnectorPluginManager::stop);
    connect(ui->playControls, &PlayControlsWidget::pause, m_connectorManager, &ConnectorPluginManager::pause);
    connect(ui->playControls, &PlayControlsWidget::startAll, m_connectorManager, &ConnectorPluginManager::startAll);
    connect(ui->playControls, &PlayControlsWidget::pauseAll, m_connectorManager, &ConnectorPluginManager::pauseAll);
    connect(ui->playControls, &PlayControlsWidget::stopAll, m_connectorManager, &ConnectorPluginManager::stopAll);
}

void MainWindow::setupMenus()
{
    m_recentFilesMenu = new QRecentFilesMenu(tr("Recent Files"), ui->menuFile);
    ui->menuFile->insertMenu(ui->actionQuit, m_recentFilesMenu);
    ui->menuFile->insertSeparator(ui->actionQuit);

    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->connectorsDock->toggleViewAction());
    ui->menuView->addAction(ui->filtersDock->toggleViewAction());

    foreach (ToolPluginInterface *tpi, m_tools) {
        QAction *action = ui->menuTools->addAction(tpi->pluginIcon(), tpi->pluginName());
        connect(action, &QAction::triggered,
                tpi, &ToolPluginInterface::show);
        connect(this, &MainWindow::languageChanged,
                this, [=](){
            action->setText(tpi->pluginName());
        });
    }
    ui->menuTools->menuAction()->setVisible(!m_tools.isEmpty());
}

void MainWindow::updateViewerSources(){
    m_filterManager->updateConnectorsList(m_connectorManager->pluginInformation());
    QVector<PluginInformation> sources;
    sources << m_connectorManager->pluginInformation()
            << m_filterManager->pluginInformation();
    m_viewerManager->updateConnectorsList(sources);
}

void MainWindow::setupShortcuts()
{

}

void MainWindow::initLanguage()
{
    // format systems language
    QString defaultLocale = m_settings.value("currentLanguage", QLocale::system().name()).toString();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    m_connectorManager->loadTranslator(defaultLocale);
    m_viewerManager->loadTranslator(defaultLocale);
    m_filterManager->loadTranslator(defaultLocale);
    loadToolsTranslator(defaultLocale);
    loadSystemsTranslator(defaultLocale);

    if (translator.load(defaultLocale, ":/translations")) {
        qApp->installTranslator(&translator);
        if (defaultLocale == "ja") {
            ui->actionJapanese->setChecked(true);
        }
        //        if (defaultLocale == "fr") {
        //            ui->actionFrancais->setChecked(true);
        //        }
        if (defaultLocale == "en") {
            ui->actionEnglish->setChecked(true);
        }
    }
}

void MainWindow::updatePlayControls(bool activeState, bool startedState, bool pausedState)
{
    bool isMoreActive = m_connectorManager->connectedCount() > 1;
    ui->playControls->updatePlayControls(activeState, startedState, pausedState, isMoreActive);
}

void MainWindow::actionOpenTriggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open log file"),
                QDir::currentPath(),
                tr("Log file (*.ubh)"));
    if (!filename.isNull()) {
        QFileInfo fi(filename);
        QDir::setCurrent(fi.absolutePath());

        loadFile(filename);
    }
}

void MainWindow::loadFile(QString filename)
{
    m_connectorManager->loadFile(filename);
    m_recentFilesMenu->addRecentFile(filename);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    foreach(QUrl url, event->mimeData()->urls()){
        QString filename = url.toLocalFile();
        QFileInfo fi(filename);
        if (fi.suffix().toUpper() == "UBH") {
            event->acceptProposedAction();
            return;
        }
        if (fi.fileName().toLower() == "debug.hdf") {
            event->acceptProposedAction();
            return;
        }
        if (fi.fileName().toLower() == "fram_default_table.txt") {
            event->acceptProposedAction();
            return;
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    foreach(QUrl url, event->mimeData()->urls()) {
        QString filename = url.toLocalFile();
        QFileInfo fi(filename);
        if (fi.suffix().toUpper() == "UBH") {
            event->acceptProposedAction();
            loadFile(filename);
            break;
        }
        if (fi.fileName().toLower() == "debug.hdf") {
            event->acceptProposedAction();
            QFile::copy(filename, QApplication::applicationDirPath() + "/debug.hdf");
            QMessageBox::information(this, QApplication::applicationName(),
                                     "Debug file copied. Restart the application to activate debug session.");
            break;
        }
        if (fi.fileName().toLower() == "fram_default_table.txt") {
            event->acceptProposedAction();
            QFile::copy(filename, QApplication::applicationDirPath() + "/fram_default_table.txt");
            QMessageBox::information(this, QApplication::applicationName(),
                                     "Debug file copied. Restart the application to activate debug session.");
            break;
        }
    }
}

void MainWindow::checkForLoadFile()
{
    QStringList cmdline_args = QCoreApplication::arguments();
    if ((cmdline_args.size() == 2) && !cmdline_args[1].startsWith("-")) {
        loadFile(cmdline_args[1]);
    }
}

void MainWindow::UBHFileFormat()
{
    QString filename = QApplication::applicationDirPath() + "/ubh_file_format.pdf";
    QFileInfo fi(filename);
    if(!fi.exists()){
        //        QFile::remove(QApplication::applicationDirPath() + "/ubh_file_format.pdf");
        QFile::copy(":/files/ubh_format" , filename);
    }
    if(!QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(filename), QUrl::TolerantMode))){
        QMessageBox::warning(this, QApplication::applicationName(),
                             tr("Could not open help file") + ":\n" +
                             filename);
    }
}




void MainWindow::setupConnections()
{
    //--------------------

    connect(m_connectorManager, &ConnectorPluginManager::connectorDataReady,
            m_filterManager, &FilterPluginManager::onConnectorDataReady);

    //----------------------------------------------
    connect(m_connectorManager, &ConnectorPluginManager::currentPluginChanged,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::currentPluginChanged,
            this, &MainWindow::switchViewer);
    connect(m_connectorManager, &ConnectorPluginManager::pluginAdded,
            this,&MainWindow::pluginAdded);
    connect(m_connectorManager, &ConnectorPluginManager::pluginRemoved,
            this, &MainWindow::pluginRemoved);
    connect(m_connectorManager, &ConnectorPluginManager::connectorConnected,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::connectorConnected,
            this, &MainWindow::switchViewer);
    connect(m_connectorManager, &ConnectorPluginManager::connectorDisconnected,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::connectorStarted,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::connectorPaused,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::connectorStopped,
            this, &MainWindow::updateControls);
    connect(m_connectorManager, &ConnectorPluginManager::connectorDataReady,
            this, &MainWindow::deviceDataReady);

    connect(m_connectorManager, &ConnectorPluginManager::fileOpened,
            m_recentFilesMenu, &QRecentFilesMenu::addRecentFile);

    connect(m_filterManager, &FilterPluginManager::pluginAdded,
            this,&MainWindow::pluginAdded);
    connect(m_filterManager, &FilterPluginManager::pluginRemoved,
            this, &MainWindow::pluginRemoved);
    connect(m_filterManager, &FilterPluginManager::connectorDataReady,
            this, &MainWindow::deviceDataReady);

    connect(ui->actionAbout_Qt, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));

//    connect(&m_settingsWindow, &SettingsWindow::error,
//            this, &MainWindow::notifyError);
//    connect(&m_settingsWindow, &SettingsWindow::information,
//            this, &MainWindow::notifyInformation);
//    connect(&m_settingsWindow, &SettingsWindow::warning,
//            this, &MainWindow::notifyWarning);

    connect(ui->actionPlugin_Information, &QAction::triggered,
            m_pluginDialog, &QWidget::show);

    connect(ui->actionOpen, &QAction::triggered,
            this, &MainWindow::actionOpenTriggered);
    //------------Import export connections

    connect(m_recentFilesMenu, &QRecentFilesMenu::recentFileTriggered,
            this, &MainWindow::loadFile);

    connect(ui->actionUBH_file_format, &QAction::triggered,
            this, &MainWindow::UBHFileFormat);

//    connect(ui->actionSettings, &QAction::triggered,
//            &m_settingsWindow, &QWidget::show);

    //--------------------------------
    connect(m_viewerManager, &MutiViewerWidget::fullScreenToggled,
            ui->connectorsDock, &QWidget::setVisible);
    connect(ui->connectorsDock, &QDockWidget::visibilityChanged,
            m_viewerManager, &MutiViewerWidget::setFullScreenChecked);

    //    QDialog *dlg = new QDialog();
    //    QHBoxLayout *dlg_layout = new QHBoxLayout(dlg);
    //    dlg_layout->setContentsMargins(0, 0, 0, 0);
    //    dlg_layout->addWidget(m_viewerSplitter);
    //    dlg->setLayout(dlg_layout);
    //    dlg->showFullScreen();



    //---------------Menues connection

    connect(ui->actionAbout_UrgBenri, &QAction::triggered,
            this, &MainWindow::aboutButtonClicked);

    QActionGroup* langGroup = new QActionGroup(ui->menuLanguage);
    langGroup->setExclusive(true);
    langGroup->addAction(ui->actionEnglish);
    langGroup->addAction(ui->actionFrancais);
    langGroup->addAction(ui->actionJapanese);

    connect(ui->actionEnglish, &QAction::toggled,
            this, &MainWindow::actionEnglishToggled);
    connect(ui->actionFrancais, &QAction::toggled,
            this, &MainWindow::actionFrancaisToggled);
    connect(ui->actionJapanese, &QAction::toggled,
            this, &MainWindow::actionJapaneseToggled);

    connect(qApp, &QApplication::aboutToQuit,
            this, [&](){
        saveState();
        foreach (SystemPluginInterface *spi, m_systems) {
             spi->finalize();
        }
    });
}

void MainWindow::restoreState()
{
    m_settings.beginGroup(m_pluginDialog->metaObject()->className());
    m_pluginDialog->restoreState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_connectorManager->metaObject()->className());
    m_connectorManager->restoreState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_filterManager->metaObject()->className());
    m_filterManager->restoreState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_viewerManager->metaObject()->className());
    m_viewerManager->restoreState(m_settings);
    m_settings.endGroup();

//    m_settings.beginGroup(m_settingsWindow.metaObject()->className());
//    m_settingsWindow.restoreState(m_settings);
//    m_settings.endGroup();

    m_recentFilesMenu->restoreState(m_settings.value("recentFiles").toByteArray());
    QDir::setCurrent(m_settings.value("currentFolder", QDir::homePath()).toString());

    ui->actionEnglish->setChecked(m_settings.value("language_english", true).toBool());
    ui->actionFrancais->setChecked(m_settings.value("language_french", false).toBool());
    ui->actionJapanese->setChecked(m_settings.value("language_japanese", false).toBool());


    ui->connectorsDock->setVisible(m_settings.value("connector_visible", true).toBool());
    if(m_filterManager->pluginCount() > 0){
        ui->filtersDock->setVisible(m_settings.value("filter_visible", false).toBool());
    }

    foreach (ToolPluginInterface *tool, m_tools) {
        m_settings.beginGroup(tool->metaObject()->className());
        tool->restoreState(m_settings);
        m_settings.endGroup();
    }

    restoreGeometry(m_settings.value("widgetGeometry").toByteArray());
}

void MainWindow::saveState()
{
    //    ui->viewerSettingsWidget->saveSettings();
    m_settings.setValue("widgetGeometry", saveGeometry());

    m_settings.beginGroup(m_pluginDialog->metaObject()->className());
    m_pluginDialog->saveState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_connectorManager->metaObject()->className());
    m_connectorManager->saveState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_filterManager->metaObject()->className());
    m_filterManager->saveState(m_settings);
    m_settings.endGroup();

    m_settings.beginGroup(m_viewerManager->metaObject()->className());
    m_viewerManager->saveState(m_settings);
    m_settings.endGroup();

//    m_settings.beginGroup(m_settingsWindow.metaObject()->className());
//    m_settingsWindow.saveState(m_settings);
//    m_settings.endGroup();

    m_settings.setValue("recentFiles", m_recentFilesMenu->saveState());
    m_settings.setValue("currentFolder", QDir::currentPath());

    m_settings.setValue("ethernet_debug_mode", false);
    m_settings.setValue("serial_debug_mode", false);

    m_settings.setValue("language_english", ui->actionEnglish->isChecked());
    m_settings.setValue("language_french", ui->actionFrancais->isChecked());
    m_settings.setValue("language_japanese", ui->actionJapanese->isChecked());


    m_settings.setValue("connector_visible", ui->connectorsDock->isVisible());
    if(m_filterManager->pluginCount() > 0){
        m_settings.setValue("filter_visible", ui->filtersDock->isVisible());
    }

    foreach (ToolPluginInterface *tool, m_tools) {
        m_settings.beginGroup(tool->metaObject()->className());
        tool->saveState(m_settings);
        m_settings.endGroup();
    }

    m_settings.sync();
}

void MainWindow::aboutButtonClicked()
{
    QMessageBox::about(this, QApplication::applicationName(),
                       QString(QApplication::applicationName()) + QString(" by ") + QString(QApplication::organizationName()) + QString("\n") +
                       QString(tr("Version") + ": ") + QString(QApplication::applicationVersion()) + QString("\n") +
                       QString(tr("Revision") + ": ") + QString(APP_REVISION) + QString("\n") +
                       QString(tr("Date") + ": ") + QString(APP_VERSION_DATE) + QString("\n") +
                       QString("Report bugs to kristou@hokuyo-aut.jp"));
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange: {
            if(ui) ui->retranslateUi(this);
            if(m_recentFilesMenu) m_recentFilesMenu->setTitle(tr("Recent Files"));
            emit languageChanged();
        }break;
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "MainWindow::closeEvent";
    saveState();
    foreach (SystemPluginInterface *spi, m_systems) {
         spi->finalize();
    }
    QMainWindow::closeEvent(event);
    event->accept();

    exit(0);
}

void MainWindow::loadToolsTranslator(const QString &locale)
{
    for (int i = 0; i < m_tools.size(); ++i) {
        ToolPluginInterface* tpi = m_tools[i];
        if (tpi) tpi->loadTranslator(locale);
    }
}

void MainWindow::loadSystemsTranslator(const QString &locale)
{
    for (int i = 0; i < m_systems.size(); ++i) {
        SystemPluginInterface* spi = m_systems[i];
        if (spi) spi->loadTranslator(locale);
    }
}

void MainWindow::actionEnglishToggled(bool state)
{
    if (state) {
        m_connectorManager->loadTranslator("en");
        m_filterManager->loadTranslator("en");
        m_viewerManager->loadTranslator("en");
        loadToolsTranslator("en");
        loadSystemsTranslator("en");
        qApp->removeTranslator(&translator);
        if (translator.load("en", ":/translations")) {
            qApp->installTranslator(&translator);
            m_settings.setValue("currentLanguage", "en_US");
        }
    }
}

void MainWindow::actionFrancaisToggled(bool state)
{
    if (state) {
        m_connectorManager->loadTranslator("fr");
        m_filterManager->loadTranslator("fr");
        m_viewerManager->loadTranslator("fr");
        loadToolsTranslator("fr");
        loadSystemsTranslator("fr");
        qApp->removeTranslator(&translator);
        if (translator.load("fr", ":/translations")) {
            qApp->installTranslator(&translator);
            m_settings.setValue("currentLanguage", "fr_FR");

        }
    }
}

void MainWindow::actionJapaneseToggled(bool state)
{
    if (state) {
        m_connectorManager->loadTranslator("ja");
        m_filterManager->loadTranslator("ja");
        m_viewerManager->loadTranslator("ja");
        loadToolsTranslator("ja");
        loadSystemsTranslator("ja");
        qApp->removeTranslator(&translator);
        if (translator.load("ja", ":/translations")) {
            qApp->installTranslator(&translator);
            QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/m_settings.ini", QSettings::IniFormat);
            m_settings.setValue("currentLanguage", "ja_JP");

        }
    }
}


void MainWindow::pluginAdded(const QString &id)
{
    Q_UNUSED(id);

    updateViewerSources();
}

void MainWindow::pluginRemoved(const QString &id)
{
    Q_UNUSED(id);

    updateViewerSources();
}

void MainWindow::updateControls(const QString &id)
{
    qDebug() << "MainWindow::updateControls";
    ConnectorState state = m_connectorManager->connectorState(id);
    updatePlayControls(state.connected,
                       state.started,
                       state.paused);
    if(!state.started){
        if(!state.paused){
            clearViewer(id);
        }
    }
}

void MainWindow::switchViewer(const QString &id){
    if((m_connectorManager->connectedCount() == 1)
            && (m_viewerManager->visibleViewerCount() == 1)){
        m_viewerManager->setMainViewerSource(id);
    }
}

void MainWindow::deviceDataReady(const QString &id, const PluginDataStructure &data)
{
    m_viewerManager->addMeasurementData(id, data);
}

void MainWindow::clearViewer(const QString &id)
{
    QTimer *timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=](){
        m_viewerManager->clear(id);
        timer->deleteLater();
    });
    timer->start(100);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow";
    delete ui;
}

