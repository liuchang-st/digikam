/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-12-20
 * Description : a widget to display a welcome page 
 *               on root album.
 * 
 * Copyright 2006 by Gilles Caulier
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

#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <khtml_part.h>

namespace Digikam
{

class WelcomePage : public KHTMLPart
{
    Q_OBJECT

public:

    WelcomePage(QWidget* parent);
    ~WelcomePage();

private:

    QCString fileToString(const QString &aFileName);
    QString  infoPage();

private slots:

    void slotUrlOpen(const KURL &);
};

}  // namespace Digikam

#endif /* WELCOMEPAGE_H */
