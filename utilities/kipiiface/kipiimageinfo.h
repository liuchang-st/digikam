/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-02
 * Description : class to get/set image information/properties
 *               in a digiKam album.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2005 by Ralf Holzer <ralf at well.com>
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef KIPIIMAGEINFO_H
#define KIPIIMAGEINFO_H

// Qt includes

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QMap>

// KDE includes

#include <kurl.h>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>
#include <libkipi/imageinfo.h>
#include <libkipi/imageinfoshared.h>

// Local includes

#include "imageinfo.h"

namespace Digikam
{

class PAlbum;

class KipiImageInfo : public KIPI::ImageInfoShared
{
public:

    KipiImageInfo(KIPI::Interface* const interface, const KUrl& url);
    ~KipiImageInfo();

#if KIPI_VERSION >= 0x010300
    QString   name();
    void      setName(const QString&);
#else
    /// Deprecated methods: for KIPI title here want mean "filename", not comment Title property.
    QString   title();
    void      setTitle(const QString&);
#endif // KIPI_VERSION >= 0x010300

    /// Manage default comment property
    QString   description();
    void      setDescription(const QString&);

    QDateTime time(KIPI::TimeSpec);
    void      setTime(const QDateTime& date, KIPI::TimeSpec spec = KIPI::FromInfo );

    int       angle();
    void      setAngle(int orientation);

#if KIPI_VERSION >= 0x010200
    void cloneData(ImageInfoShared* const other);
#else
    void cloneData(ImageInfoShared* other);
#endif // KIPI_VERSION >= 0x010200

    QMap<QString, QVariant> attributes();
    void addAttributes(const QMap<QString, QVariant>& res);
    void delAttributes(const QStringList& res);
    void clearAttributes();

private:

    PAlbum* parentAlbum() const;

private:

    ImageInfo m_info;
};

}  // namespace Digikam

#endif  // KIPIIMAGEINFO_H
