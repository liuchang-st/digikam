/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2006-02-01
 * Description : 
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

// Qt includes.
 
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qframe.h>
#include <qhbuttongroup.h> 
#include <qpushbutton.h>

// KDE includes.

#include <kdialog.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kstandarddirs.h>

// Local includes

#include "imageguidewidget.h"
#include "imagewidget.h"

namespace Digikam
{

class ImageWidgetPriv
{
public:

    ImageWidgetPriv()
    {
        previewButtons = 0;
        previewWidget  = 0;
    }

    QHButtonGroup    *previewButtons;

    ImageGuideWidget *previewWidget;
};

ImageWidget::ImageWidget(QWidget *parent, const QString& previewWhatsThis)
           : QWidget(parent)
{
    d = new ImageWidgetPriv;

    QLabel *title = new QLabel(i18n("Preview Mode:"), this);
    QGridLayout* grid = new QGridLayout( this, 1, 2, KDialog::marginHint(),
                                         KDialog::spacingHint());

    d->previewButtons = new QHButtonGroup(this);
    d->previewButtons->setExclusive(true);
    d->previewButtons->setInsideMargin( 0 );
    d->previewButtons->setFrameShape(QFrame::NoFrame);

    QPushButton *previewOriginalButton = new QPushButton( d->previewButtons );
    d->previewButtons->insert(previewOriginalButton, ImageGuideWidget::PreviewOriginalImage);
    KGlobal::dirs()->addResourceType("original", KGlobal::dirs()->kde_default("data") + "digikam/data");
    QString directory = KGlobal::dirs()->findResourceDir("original", "original.png");
    previewOriginalButton->setPixmap( QPixmap( directory + "original.png" ) );
    previewOriginalButton->setToggleButton(true);
    QToolTip::add( previewOriginalButton, i18n( "<p>If you enable this option, you will see "
                                                "the original image." ) );

    QPushButton *previewBothButtonVert = new QPushButton( d->previewButtons );
    d->previewButtons->insert(previewBothButtonVert, ImageGuideWidget::PreviewBothImagesVert);
    KGlobal::dirs()->addResourceType("duplicatebothvert", KGlobal::dirs()->kde_default("data") + "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("duplicatebothvert", "duplicatebothvert.png");
    previewBothButtonVert->setPixmap( QPixmap( directory + "duplicatebothvert.png" ) );
    previewBothButtonVert->setToggleButton(true);
    QToolTip::add( previewBothButtonVert, i18n( "<p>If you enable this option, the preview area will "
                                                "be separated vertically." ) );

    QPushButton *previewBothButtonHorz = new QPushButton( d->previewButtons );
    d->previewButtons->insert(previewBothButtonHorz, ImageGuideWidget::PreviewBothImagesHorz);
    KGlobal::dirs()->addResourceType("duplicatebothhorz", KGlobal::dirs()->kde_default("data") + "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("duplicatebothhorz", "duplicatebothhorz.png");
    previewBothButtonHorz->setPixmap( QPixmap( directory + "duplicatebothhorz.png" ) );
    previewBothButtonHorz->setToggleButton(true);
    QToolTip::add( previewBothButtonHorz, i18n( "<p>If you enable this option, the preview area will "
                                                "be separated horizontally." ) );

    QPushButton *previewtargetButton = new QPushButton( d->previewButtons );
    d->previewButtons->insert(previewtargetButton, ImageGuideWidget::PreviewTargetImage);
    KGlobal::dirs()->addResourceType("target", KGlobal::dirs()->kde_default("data") + "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("target", "target.png");
    previewtargetButton->setPixmap( QPixmap( directory + "target.png" ) );
    previewtargetButton->setToggleButton(true);
    QToolTip::add( previewtargetButton, i18n( "<p>If you enable this option, you will see "
                                              "the target image." ) );

    QFrame *frame    = new QFrame(this);
    frame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
    QVBoxLayout* l   = new QVBoxLayout(frame, 5, 0);
    d->previewWidget = new ImageGuideWidget(480, 320, frame, true, 
                                           ImageGuideWidget::PickColorMode,
                                           Qt::red, 1, false,
                                           ImageGuideWidget::TargetPreviewImage);
    QWhatsThis::add( d->previewWidget, previewWhatsThis);
    l->addWidget(d->previewWidget, 0);

    grid->addMultiCellWidget(title, 0, 0, 0, 0);
    grid->addMultiCellWidget(d->previewButtons, 0, 0, 1, 1);
    grid->addMultiCellWidget(frame, 1, 1, 0, 2);
    grid->setRowStretch(1, 10);
    grid->setColStretch(2, 10);

    connect(d->previewWidget, SIGNAL(signalResized()),
            this, SIGNAL(signalResized()));

    connect(d->previewWidget, SIGNAL(spotPositionChanged( const Digikam::DColor &, bool, const QPoint & )),
            this, SIGNAL(spotPositionChanged( const Digikam::DColor &, bool, const QPoint & )));

    connect(d->previewButtons, SIGNAL(released(int)),
            d->previewWidget, SLOT(slotChangeRenderingPreviewMode(int)));

    d->previewButtons->setButton(ImageGuideWidget::PreviewBothImagesVert);
    d->previewWidget->slotChangeRenderingPreviewMode(ImageGuideWidget::PreviewBothImagesVert);
}

ImageWidget::~ImageWidget()
{
    delete d;
}

ImageIface* ImageWidget::imageIface()
{
    return d->previewWidget->imageIface();
}

void ImageWidget::updatePreview()
{
    d->previewWidget->updatePreview();
}

void ImageWidget::slotChangeGuideColor(const QColor &color)
{
    d->previewWidget->slotChangeGuideColor(color);
}

void ImageWidget::slotChangeGuideSize(int size)
{
    d->previewWidget->slotChangeGuideSize(size);
}

}  // namespace Digikam

#include "imagewidget.moc"
