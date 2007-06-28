/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-11-11
 * Description : a popup menu with a decorative graphic banner
 *               at the left border.
 * 
 * Copyright (C) 1996-2000 the kicker authors.
 * Copyright (C) 2005 Mark Kretschmann <markey@web.de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DPOPUPMENU_H
#define DPOPUPMENU_H

// Qt includes.

#include <Q3PopupMenu>
#include <QColor>
#include <QImage>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>

// Local includes.

#include "digikam_export.h"

class QSize;

namespace Digikam
{

class DIGIKAM_EXPORT DPopupMenu : public Q3PopupMenu
{
    Q_OBJECT

public:

    DPopupMenu(QWidget *parent = 0, const char *name = 0);

    int sidePixmapWidth() const { return s_dpopupmenu_sidePixmap.width(); }

private:

    /** Loads and prepares the sidebar image */
    void generateSidePixmap();

    /** Returns the available size for the image */
    QRect sideImageRect() const;

    /** Calculates a color that matches the current colorscheme */
    QColor calcPixmapColor();

    void setMinimumSize( const QSize& s );
    void setMaximumSize( const QSize& s );
    void setMinimumSize( int w, int h );
    void setMaximumSize( int w, int h );

    void resizeEvent( QResizeEvent* e );
    void resize( int width, int height );

    void paintEvent( QPaintEvent* e );

private:

    static QImage s_dpopupmenu_sidePixmap;
    static QColor s_dpopupmenu_sidePixmapColor;
};

}  // namespace Digikam

#endif /*DPOPUPMENU_H*/
