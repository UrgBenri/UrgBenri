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

#include "PluginManager.h"

#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>

const QString LogHeader = QStringLiteral("PluginManager");

PluginManager::PluginManager(QObject *parent)
    : QAbstractTableModel(parent)
{
    setObjectName(LogHeader);
#ifdef Q_OS_WIN
    m_pluginPaths << QApplication::applicationDirPath()+"/plugins";
#endif
#ifdef Q_OS_LINUX
    m_pluginPaths << QApplication::applicationDirPath()+"/plugins";
#endif
#ifdef Q_OS_OSX
    m_pluginPaths << QApplication::applicationDirPath()+"/../../PlugIns";
#endif
    m_pluginPaths << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/plugins";

    foreach (const QString &pluginPath, m_pluginPaths) {
        loadPlugins(pluginPath);
    }

    foreach (PluginInfo *info, m_plugins.values()) {
        if(info){
            info->instance()->onLoad(this);
        }
    }
}

PluginManager::~PluginManager()
{
    foreach (PluginInfo* plugin, m_plugins.values()) {
        plugin->instance()->onUnload();
        delete plugin;
    }
}

void PluginManager::loadPlugins(const QString &pluginPath)
{
    QDir pluginsDir(pluginPath);
    QStringList filters;

#ifdef Q_OS_WIN
    filters << "*.dll";
#endif
#ifdef Q_OS_LINUX
    filters << "*.so";
#endif
#ifdef Q_OS_OSX
    filters << "*.dylib";
#endif

    foreach( QString fileName, pluginsDir.entryList(filters, QDir::Files)) {
        QString absolutePath = pluginsDir.absoluteFilePath(fileName);
        if(QLibrary::isLibrary(absolutePath)){
            loadPlugin(absolutePath);
        }
    }
}

QStringList PluginManager::pluginPaths() const
{
    return m_pluginPaths;
}

void PluginManager::addPluginPath(const QString &pluginPath)
{
    m_pluginPaths << pluginPath;
}

void PluginManager::saveState(QSettings &settings)
{
    foreach (const QString &key, m_plugins.keys()) {
        settings.setValue(key, m_plugins[key]->isEnabled());
    }
}

void PluginManager::restoreState(QSettings &settings)
{
    foreach (const QString &key, m_plugins.keys()) {
        m_plugins[key]->setEnabled(settings.value(key, !m_plugins[key]->instance()->pluginIsExperimental()).toBool());
    }
}

int PluginManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_plugins.size();
}

int PluginManager::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return LastColumn;
}

QVariant PluginManager::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && index.row() < m_plugins.size()){

        switch (index.column()) {
            case EnabledColumn:
                switch (role) {
                    case Qt::CheckStateRole: return m_plugins.values().at(index.row())->isEnabled() ?
                                    Qt::Checked : Qt::Unchecked;
                    case Qt::DisplayRole: return "";
                }
                break;
            case IconColumn:
                switch (role) {
                    case Qt::DecorationRole: return m_plugins.values().at(index.row())->instance()->pluginIcon();
                    case Qt::DisplayRole: return "";
                    case Qt::TextAlignmentRole: return Qt::AlignCenter;
                }
                break;
            case NameColumn:
                switch (role) {
                    case Qt::DisplayRole: return m_plugins.values().at(index.row())->instance()->pluginName();
                    case Qt::TextAlignmentRole: return Qt::AlignLeft;
                }
                break;
            case VersionColumn:
                switch (role) {
                    case Qt::DisplayRole: return m_plugins.values().at(index.row())->instance()->pluginVersion().toString();
                    case Qt::TextAlignmentRole: return Qt::AlignLeft;
                }
                break;
            case AuthorColumn:
                switch (role) {
                    case Qt::DisplayRole: return m_plugins.values().at(index.row())->instance()->pluginAuthorName();
                    case Qt::TextAlignmentRole: return Qt::AlignLeft;
                }
                break;
            case ContactColumn:
                switch (role) {
                    case Qt::DisplayRole: return m_plugins.values().at(index.row())->instance()->pluginAuthorContact();
                    case Qt::TextAlignmentRole: return Qt::AlignLeft;
                }
                break;
            case DescriptionColumn:
                switch (role) {
                    case Qt::DisplayRole: return m_plugins.values().at(index.row())->instance()->pluginDescription();
                    case Qt::TextAlignmentRole: return Qt::AlignLeft;
                }
                break;
            default:
                break;
        }
    }
    return QVariant();
}

bool PluginManager::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && index.row() < m_plugins.size()){
        switch (index.column()) {
            case EnabledColumn: {
                m_plugins.values().at(index.row())->setEnabled(value.toBool());
                emit restartRequired();
                return true;
            }
        }
    }
    return false;
}

QVariant PluginManager::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal){
        switch (section) {
            case IconColumn: return tr("Icon");
            case NameColumn: return tr("Name");
            case VersionColumn: return tr("Version");
            case AuthorColumn: return tr("Author");
            case ContactColumn: return tr("Contact");
            case DescriptionColumn: return tr("Description");
        }
    }

    return QVariant();
}

Qt::ItemFlags PluginManager::flags(const QModelIndex &index) const
{
    if(index.isValid() && index.column() == EnabledColumn){
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }

    return QAbstractTableModel::flags(index);
}

GeneralPluginInterface *PluginManager::makePlugin(const QString &id, QWidget *parent)
{
    if(m_plugins.contains(id) && m_plugins[id]->isEnabled()){
        GeneralPluginInterface *object = m_plugins[id]->instance()->createPlugin(parent);
        if(object){
            object->onLoad(this);
            connect(object, SIGNAL(error(const QString&,const QString&)),
                    this, SIGNAL(error(const QString&,const QString&)));
            connect(object, SIGNAL(information(const QString&,const QString&)),
                    this, SIGNAL(information(const QString&,const QString&)));
            connect(object, SIGNAL(warning(const QString&,const QString&)),
                    this, SIGNAL(warning(const QString&,const QString&)));
        }
        return object;
    }
    return Q_NULLPTR;
}

GeneralPluginInterface *PluginManager::getPlugin(const QString &id)
{
    if(m_plugins.contains(id) && m_plugins[id]->isEnabled()){
        return m_plugins[id]->instance();
    }
    return Q_NULLPTR;
}

bool PluginManager::loadPlugin(const QString &filename)
{
    try{
        QPluginLoader *loader = new QPluginLoader(filename, this);
        if(loader){
            QString pluginId = loader->metaData().value("IID").toString();
            if(m_plugins.contains(pluginId)) return false;

            QObject *pluginInstance = loader->instance();
            if (pluginInstance){
                GeneralPluginInterface *generalPlugin = qobject_cast<GeneralPluginInterface*>(pluginInstance);
                if(generalPlugin){
                    m_plugins[pluginId] = new PluginInfo(loader, generalPlugin);
                    connect(generalPlugin, SIGNAL(error(const QString&,const QString&)),
                            this, SIGNAL(error(const QString&,const QString&)));
                    connect(generalPlugin, SIGNAL(information(const QString&,const QString&)),
                            this, SIGNAL(information(const QString&,const QString&)));
                    connect(generalPlugin, SIGNAL(warning(const QString&,const QString&)),
                            this, SIGNAL(warning(const QString&,const QString&)));
                    return true;
                }
                else{
                    qCritical() << "Not supported plugin" << filename;
                    qCritical() << pluginId;
                }
            }
            else{
                qCritical() << loader->errorString();
            }
        }
        loader->unload();
        delete loader;
    }
    catch(std::exception &e){
        QString message = tr("Error loading plugin:%1").arg(filename) +
                tr("Error:%1").arg(e.what());
        emit error(LogHeader, message);
    }

    return false;
}

