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
#include "QRecentFilesMenu.h"

#include <QFileInfo>

static const qint32 RecentFilesMenuMagic = 0xff;

QRecentFilesMenu::QRecentFilesMenu(QWidget* parent)
    : QMenu(parent)
    , m_maxCount(5)
    , m_format(QLatin1String("%d %s"))
{
    connect(this, &QRecentFilesMenu::triggered,
            this, &QRecentFilesMenu::menuTriggered);

    setMaxCount(m_maxCount);
}

QRecentFilesMenu::QRecentFilesMenu(const QString &title, QWidget* parent)
    : QMenu(title, parent)
    , m_maxCount(5)
    , m_format(QLatin1String("%d %s"))
{
    connect(this, &QRecentFilesMenu::triggered,
            this, &QRecentFilesMenu::menuTriggered);
    setIcon(QIcon(":/icons/recent_icon"));
    setMaxCount(m_maxCount);
}

void QRecentFilesMenu::addRecentFile(const QString &fileName)
{
    m_files.removeAll(fileName);
    m_files.prepend(fileName);

    while (m_files.size() > maxCount()) {
        m_files.removeLast();
    }

    updateRecentFileActions();
}

void QRecentFilesMenu::clearMenu()
{
    m_files.clear();

    updateRecentFileActions();
}

int QRecentFilesMenu::maxCount() const
{
    return m_maxCount;
}

void QRecentFilesMenu::setFormat(const QString &format)
{
    if (m_format == format) {
        return;
    }
    m_format = format;

    updateRecentFileActions();
}

const QString &QRecentFilesMenu::format() const
{
    return m_format;
}

QByteArray QRecentFilesMenu::saveState() const
{
    int version = 0;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qint32(RecentFilesMenuMagic);
    stream << qint32(version);
    stream << m_files;

    return data;
}

bool QRecentFilesMenu::restoreState(const QByteArray &state)
{
    int version = 0;
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    qint32 marker;
    qint32 v;

    stream >> marker;
    stream >> v;
    if (marker != RecentFilesMenuMagic || v != version) {
        return false;
    }

    stream >> m_files;

    updateRecentFileActions();

    return true;
}

void QRecentFilesMenu::setMaxCount(int count)
{
    m_maxCount = count;

    updateRecentFileActions();
}

void QRecentFilesMenu::menuTriggered(QAction* action)
{
    if (action->data().isValid()) {
        emit recentFileTriggered(action->data().toString());
    }
}

void QRecentFilesMenu::updateRecentFileActions()
{
    int numRecentFiles = qMin(m_files.size(), maxCount());

    clear();

    for (int i = 0; i < numRecentFiles; ++i) {
        QString strippedName = QFileInfo(m_files[i]).fileName();

        QString text = m_format;
        text.replace(QLatin1String("%d"), QString::number(i + 1));
        text.replace(QLatin1String("%s"), strippedName);

        QAction* recentFileAct = addAction(text);
        recentFileAct->setData(m_files[i]);
    }
    addSeparator();
    addAction(QIcon(":/icons/clear_icon"), tr("Clear Menu"), this, SLOT(clearMenu()));

    setEnabled(numRecentFiles > 0);
}

