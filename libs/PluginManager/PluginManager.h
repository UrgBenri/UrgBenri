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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QPluginLoader>
#include <QAbstractTableModel>
#include "PluginInfo.h"
#include "PluginManagerInterface.h"

class PluginManager : public QAbstractTableModel, public PluginManagerInterface
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = 0);
    virtual ~PluginManager();

    typedef enum TableColumn{
        EnabledColumn = 0,
        IconColumn,
        NameColumn,
        VersionColumn,
        AuthorColumn,
        ContactColumn,
        DescriptionColumn,
        LastColumn,
    }TableColumn;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual GeneralPluginInterface *makePlugin(const QString &id, QWidget *parent) Q_DECL_OVERRIDE;
    virtual GeneralPluginInterface *getPlugin(const QString &id) Q_DECL_OVERRIDE;

    template <typename Interface>
    QVector<Interface *> getPlugins(QWidget *parent = 0);

    bool loadPlugin(const QString &filename);
    void loadPlugins(const QString &pluginPaths);

    QStringList pluginPaths() const;
    void addPluginPath(const QString &pluginPaths);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

signals:
    void restartRequired();
    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);

public slots:

private:
    QStringList m_pluginPaths;
    QMap<QString, PluginInfo*> m_plugins;
};

template <typename Interface>
QVector<Interface *> PluginManager::getPlugins(QWidget *parent)
{
    QVector<Interface *> result;
    foreach (PluginInfo *plug, m_plugins.values()) {
        if(plug->isEnabled()){
            Interface *pluginInterface = qobject_cast<Interface *>(plug->instance());
            if(pluginInterface){
                GeneralPluginInterface *object = pluginInterface->createPlugin(parent);
                if(object){
                    object->onLoad(this);
                    result << qobject_cast<Interface *>(object);
                }
            }
        }
    }

    qSort(result.begin(), result.end(),
          [](GeneralPluginInterface *a, GeneralPluginInterface *b) -> bool
    {
        return a->pluginLoadOrder() < b->pluginLoadOrder();
    });
    return result;
}

#endif // PLUGINMANAGER_H

