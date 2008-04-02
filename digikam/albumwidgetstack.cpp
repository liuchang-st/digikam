/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 * 
 * Date        : 2006-06-13
 * Description : A widget stack to embedded album content view
 *               or the current image preview.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QFileInfo>
#include <QSplitter>
#include <QWidget>

// KDE includes.

#include <kurl.h>
#include <kconfiggroup.h>
#include <kconfig.h>
#include <khtmlview.h>
#include <kglobal.h>
#include <kapplication.h>

// Local includes.

#include "albumsettings.h"
#include "albumiconview.h"
#include "albumiconitem.h"
#include "imagepreviewview.h"
#include "imagepreviewbar.h"
#include "welcomepageview.h"
#include "mediaplayerview.h"
#include "albumwidgetstack.h"
#include "albumwidgetstack.moc"

namespace Digikam
{

class AlbumWidgetStackPriv
{

public:

    AlbumWidgetStackPriv()
    {
        albumIconView    = 0;
        imagePreviewView = 0;
        welcomePageView  = 0;
        mediaPlayerView  = 0;
        splitter         = 0;
        thumbBar         = 0;
    }

    QSplitter        *splitter;

    ImagePreviewBar  *thumbBar;

    AlbumIconView    *albumIconView;

    ImagePreviewView *imagePreviewView;

    WelcomePageView  *welcomePageView;

    MediaPlayerView  *mediaPlayerView;
};

AlbumWidgetStack::AlbumWidgetStack(QWidget *parent)
                : QStackedWidget(parent)
{
    d = new AlbumWidgetStackPriv;

    d->albumIconView    = new AlbumIconView(this);

    d->splitter         = new QSplitter(Qt::Vertical, this);
    d->imagePreviewView = new ImagePreviewView(d->splitter, this);
    d->thumbBar         = new ImagePreviewBar(d->splitter, Qt::Horizontal);

    // To prevent flicker effect with content when user change icon view filter 
    // if scrollbar appears or disapears.
    d->thumbBar->setHScrollBarMode(Q3ScrollView::AlwaysOn);

    d->splitter->setFrameStyle( QFrame::NoFrame );
    d->splitter->setFrameShadow( QFrame::Plain );
    d->splitter->setFrameShape( QFrame::NoFrame );
    d->splitter->setOpaqueResize(false);

    d->welcomePageView  = new WelcomePageView(this);
    d->mediaPlayerView  = new MediaPlayerView(this);

    insertWidget(PreviewAlbumMode, d->albumIconView);
    insertWidget(PreviewImageMode, d->splitter);
    insertWidget(WelcomePageMode,  d->welcomePageView->view());
    insertWidget(MediaPlayerMode,  d->mediaPlayerView);

    setPreviewMode(PreviewAlbumMode);
    setAttribute(Qt::WA_DeleteOnClose);

    readSettings();

    // -----------------------------------------------------------------

    connect(d->imagePreviewView, SIGNAL(signalNextItem()),
            this, SIGNAL(signalNextItem()));

    connect(d->imagePreviewView, SIGNAL(signalPrevItem()),
            this, SIGNAL(signalPrevItem()));

    connect(d->imagePreviewView, SIGNAL(signalEditItem()),
            this, SIGNAL(signalEditItem()));

    connect(d->imagePreviewView, SIGNAL(signalDeleteItem()),
            this, SIGNAL(signalDeleteItem()));

    connect(d->imagePreviewView, SIGNAL(signalBack2Album()),
            this, SIGNAL(signalBack2Album()));

    connect(d->imagePreviewView, SIGNAL(signalSlideShow()),
            this, SIGNAL(signalSlideShow()));

    connect(d->imagePreviewView, SIGNAL(signalZoomFactorChanged(double)),
            this, SLOT(slotZoomFactorChanged(double)));

    connect(d->imagePreviewView, SIGNAL(signalInsert2LightTable()),
            this, SIGNAL(signalInsert2LightTable()));

    connect(d->albumIconView, SIGNAL(signalItemsAdded()),
            this, SLOT(slotItemsAdded()));

    connect(d->albumIconView, SIGNAL(signalItemsRearranged()),
            this, SLOT(slotItemsAdded()));

    connect(d->thumbBar, SIGNAL(signalUrlSelected(const KUrl&)),
            this, SIGNAL(signalUrlSelected(const KUrl&)));
}

AlbumWidgetStack::~AlbumWidgetStack()
{
    saveSettings();
    delete d;
}

void AlbumWidgetStack::readSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("PreviewView");
    if (group.hasKey("SplitterState")) 
    {
        QByteArray state;
        state = group.readEntry("SplitterState", state);
        d->splitter->restoreState(QByteArray::fromBase64(state));
    }
}

void AlbumWidgetStack::saveSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group("PreviewView");
    group.writeEntry("SplitterState", d->splitter->saveState().toBase64());
    config->sync();
}

void AlbumWidgetStack::slotEscapePreview()
{
    if (previewMode() == MediaPlayerMode)
        d->mediaPlayerView->escapePreview();
}

AlbumIconView* AlbumWidgetStack::albumIconView()
{
    return d->albumIconView;
}

ImagePreviewView* AlbumWidgetStack::imagePreviewView()
{
    return d->imagePreviewView;
}

void AlbumWidgetStack::setPreviewItem(const ImageInfo & info, const ImageInfo &previous, const ImageInfo &next)
{
    if (info.isNull())
    {
        if (previewMode() == MediaPlayerMode)
            d->mediaPlayerView->setMediaPlayerFromUrl(KUrl());
        else if (previewMode() == PreviewImageMode)
        {
            d->imagePreviewView->setImageInfo();
        }

        // Special case to cleanup thumbbar if Image Lister do not query item accordingly to 
        // IconView Filters.
        ImageInfo current;
        if (d->albumIconView->allImageInfos(current).isEmpty())
            d->thumbBar->clear();
    }
    else
    {
        AlbumSettings *settings      = AlbumSettings::instance();
        QString currentFileExtension = QFileInfo(info.fileUrl().path()).suffix();
        QString mediaplayerfilter    = settings->getMovieFileFilter().toLower() +
                                       settings->getMovieFileFilter().toUpper() +
                                       settings->getAudioFileFilter().toLower() +
                                       settings->getAudioFileFilter().toUpper();
        if (mediaplayerfilter.contains(currentFileExtension) )
        {
            setPreviewMode(MediaPlayerMode);
            d->mediaPlayerView->setMediaPlayerFromUrl(info.fileUrl());
        }
        else
        {
            // Stop media player if running...
            if (previewMode() == MediaPlayerMode)
                setPreviewItem();

            if (previewMode() != PreviewImageMode)
                updateThumbbar();

            d->imagePreviewView->setImageInfo(info, previous, next);

            // NOTE: No need to toggle imediatly in PreviewImageMode here, 
            // because we will receive a signal for that when the image preview will be loaded.
            // This will prevent a flicker effect with the old image preview loaded in stack.
        }

        ThumbBarItem* item = d->thumbBar->findItemByUrl(info.fileUrl());
        d->thumbBar->setSelected(item);
    }
}

int AlbumWidgetStack::previewMode()
{
    return indexOf(currentWidget());
}

void AlbumWidgetStack::setPreviewMode(int mode)
{
    if (mode != PreviewAlbumMode && mode != PreviewImageMode && 
        mode != WelcomePageMode  && mode != MediaPlayerMode)
        return;

    if (mode == PreviewAlbumMode || mode == WelcomePageMode)
    {
        if (mode == PreviewAlbumMode && currentIndex() != mode)
            d->albumIconView->setFocus();
        setPreviewItem();
        setCurrentIndex(mode);
        emit signalToggledToPreviewMode(false);
    }
    else
    { 
        setCurrentIndex(mode);
    }
}

void AlbumWidgetStack::previewLoaded()
{
     emit signalToggledToPreviewMode(true);
}

void AlbumWidgetStack::slotZoomFactorChanged(double z)
{
    if (previewMode() == PreviewImageMode)
        emit signalZoomFactorChanged(z);
}

void AlbumWidgetStack::slotItemsUpdated(const KUrl::List& urls)
{
    // If item are updated from Icon View, and if we are in Preview Mode,
    // We will check if the current item preview need to be reloaded.

    if (previewMode() == PreviewAlbumMode ||
        previewMode() == WelcomePageMode  ||
        previewMode() == MediaPlayerMode)    // What we can do with media player ?
        return;

    if (urls.contains(imagePreviewView()->getImageInfo().fileUrl()))
        d->imagePreviewView->reload();

    for (KUrl::List::const_iterator it = urls.begin();
         it != urls.end(); ++it)
    {
        ThumbBarItem* foundItem = d->thumbBar->findItemByUrl(*it);
        d->thumbBar->invalidateThumb(foundItem);
    }
}

void AlbumWidgetStack::slotItemsAdded()
{
    if (previewMode() != PreviewImageMode)
        return;

    updateThumbbar();

    AlbumIconItem *iconItem = dynamic_cast<AlbumIconItem*>(d->albumIconView->currentItem());
    if (iconItem)
    {
        ThumbBarItem* item = d->thumbBar->findItemByUrl(iconItem->imageInfo().fileUrl());
        d->thumbBar->setSelected(item);
    }
}

void AlbumWidgetStack::updateThumbbar()
{
    d->thumbBar->clear();

    ImageInfo current;
    ImageInfoList list = d->albumIconView->allImageInfos(current);
    d->thumbBar->blockSignals(true);
    for (ImageInfoList::iterator it = list.begin(); it != list.end(); ++it)
    {
        new ImagePreviewBarItem(d->thumbBar, *it);
    }
    d->thumbBar->blockSignals(false);
}

void AlbumWidgetStack::increaseZoom()
{
    d->imagePreviewView->slotIncreaseZoom();
}

void AlbumWidgetStack::decreaseZoom()
{
    d->imagePreviewView->slotDecreaseZoom();
}

void AlbumWidgetStack::zoomTo100Percents()
{
    d->imagePreviewView->setZoomFactor(1.0);
}

void AlbumWidgetStack::fitToWindow()
{
    d->imagePreviewView->fitToWindow();
}

bool AlbumWidgetStack::maxZoom()
{
    return d->imagePreviewView->maxZoom();
}

bool AlbumWidgetStack::minZoom()
{
    return d->imagePreviewView->minZoom();
}

void AlbumWidgetStack::setZoomFactor(double z)
{
    d->imagePreviewView->setZoomFactor(z);
}

double AlbumWidgetStack::zoomFactor()
{
    return d->imagePreviewView->zoomFactor();
}

double AlbumWidgetStack::zoomMin()
{
    return d->imagePreviewView->zoomMin();
}

double AlbumWidgetStack::zoomMax()
{
    return d->imagePreviewView->zoomMax();
}

void AlbumWidgetStack::toggleShowBar(bool b)
{
    if (b)
        d->thumbBar->show();
    else
        d->thumbBar->hide();
}

}  // namespace Digikam
