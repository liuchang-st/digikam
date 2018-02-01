/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2014-09-30
 * Description : a tool to export items to Piwigo web service
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "piwigosession.h"

// Qt includes

#include <QString>
#include <QApplication>

// KDE includes

#include <kconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

PiwigoSession::PiwigoSession()
{
    load();
}

PiwigoSession::~PiwigoSession()
{
}

QString PiwigoSession::url() const
{
    return m_url;
}

QString PiwigoSession::username() const
{
    return m_username;
}

QString PiwigoSession::password() const
{
    return m_password;
}

void PiwigoSession::setUrl(const QString& url)
{
    m_url = url;
}

void PiwigoSession::setUsername(const QString& username)
{
    m_username = username;
}

void PiwigoSession::setPassword(const QString& password)
{
    m_password = password;
}

void PiwigoSession::load()
{
    // FIXME: sure we need this?? (perhaps YES..)
    static bool bln_loaded = false;

    if (bln_loaded)
        return;

    bln_loaded = true;

    // read config
    KConfig config;
    KConfigGroup group = config.group("Piwigo Settings");

    m_url      = group.readEntry("URL",      QString());
    m_username = group.readEntry("Username", QString());
    m_password = group.readEntry("Password", QString());
}

void PiwigoSession::save()
{
    KConfig config;
    KConfigGroup group = config.group("Piwigo Settings");

    group.writeEntry(QString::fromUtf8("URL"),      url());
    group.writeEntry(QString::fromUtf8("Username"), username());
    group.writeEntry(QString::fromUtf8("Password"), password());

    qCDebug(DIGIKAM_GENERAL_LOG) << "syncing..";
    config.sync();
}

} // namespace Digikam