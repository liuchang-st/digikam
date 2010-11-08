/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : Qt item view for images - the delegate
 *
 * Copyright (C) 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef IMAGEDELEGATE_H
#define IMAGEDELEGATE_H

// Local includes

#include "itemviewimagedelegate.h"
#include "thumbnailsize.h"

namespace Digikam
{

class ImageCategoryDrawer;
class ImageCategorizedView;
class ImageDelegateOverlay;
class ImageFilterModel;
class ImageModel;

class ImageDelegate : public ItemViewImageDelegate
{
    Q_OBJECT

public:

    ImageDelegate(ImageCategorizedView* parent);
    ~ImageDelegate();

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QPixmap pixmapForDrag(const QStyleOptionViewItem& option, const QList<QModelIndex>& indexes) const;


    ImageCategoryDrawer* categoryDrawer() const;

    virtual void setSpacing(int spacing);
    virtual void setDefaultViewOptions(const QStyleOptionViewItem& option);
    virtual bool acceptsToolTip(const QPoint& pos, const QRect& visualRect,
                                const QModelIndex& index, QRect* tooltipRect = 0) const;
    virtual bool acceptsActivation(const QPoint& pos, const QRect& visualRect,
                                   const QModelIndex& index, QRect* activationRect = 0) const;

    QRect rect() const;
    QRect ratingRect() const;
    QRect commentsRect() const;
    QRect tagsRect() const;
    QRect actualPixmapRect(qlonglong imageid) const;

public:

    // Declared as public because of use in DigikamImageDelegate class.
    class ImageDelegatePrivate;

protected:

    /** Reimplement this to set contentWidth. This is the maximum width of all
     *  content rectangles, typically excluding margins on both sides. */
    virtual void updateContentWidth();
    /** In a subclass, you need to implement this method to set up the rects
     *  for drawing. The paint() method operates depending on these rects. */
    virtual void updateRects() = 0;

    virtual void clearCaches();

    bool onActualPixmapRect(const QPoint& pos, const QRect& visualRect,
                            const QModelIndex& index, QRect* actualRect) const;
    void updateActualPixmapRect(qlonglong imageid, const QRect& rect);

    virtual void invalidatePaintingCache();
    virtual void updateSizeRectsAndPixmaps();

    ImageDelegate(ImageDelegate::ImageDelegatePrivate& dd, ImageCategorizedView* parent);

private:

    Q_DECLARE_PRIVATE(ImageDelegate)
};

} // namespace Digikam

#endif /* IMAGEDELEGATE_H */
