/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window
 *
 * Copyright (C) 2013-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dxmlguiwindow.h"

// Qt includes

#include <QString>
#include <QList>
#include <QMap>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QToolButton>
#include <QEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeySequence>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QUrl>
#include <QUrlQuery>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QResource>
#include <QStandardPaths>

// KDE includes

#include <ktogglefullscreenaction.h>
#include <ktoolbar.h>
#include <ktoggleaction.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kwindowconfig.h>
#include <ksharedconfig.h>
#include <kshortcutsdialog.h>
#include <kedittoolbar.h>

#ifdef HAVE_KNOTIFYCONFIG
#   include <knotifyconfigwidget.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "daboutdata.h"
#include "dpluginloader.h"
#include "webbrowserdlg.h"
#include "wsstarter.h"

namespace Digikam
{

class Q_DECL_HIDDEN DXmlGuiWindow::Private
{
public:

    explicit Private()
    {
        fsOptions              = FS_NONE;
        fullScreenAction       = 0;
        fullScreenBtn          = 0;
        dirtyMainToolBar       = false;
        fullScreenHideToolBars = false;
        fullScreenHideThumbBar = true;
        fullScreenHideSideBars = false;
        thumbbarVisibility     = true;
        menubarVisibility      = true;
        statusbarVisibility    = true;
        libsInfoAction         = 0;
        showMenuBarAction      = 0;
        showStatusBarAction    = 0;
        about                  = 0;
        dbStatAction           = 0;
        anim                   = 0;
    }

public:

    /**
     * Settings taken from managed window configuration to handle toolbar visibility  in full-screen mode
     */
    bool                     fullScreenHideToolBars;

    /**
     * Settings taken from managed window configuration to handle thumbbar visibility in full-screen mode
     */
    bool                     fullScreenHideThumbBar;

    /**
     * Settings taken from managed window configuration to handle toolbar visibility  in full-screen mode
     */
    bool                     fullScreenHideSideBars;

    /**
     * Full-Screen options. See FullScreenOptions enum and setFullScreenOptions() for details.
     */
    int                      fsOptions;

    /**
     * Action plug in managed window to switch fullscreen state
     */
    KToggleFullScreenAction* fullScreenAction;

    /**
     * Show only if toolbar is hidden
     */
    QToolButton*             fullScreenBtn;

    /**
     * Used by slotToggleFullScreen() to manage state of full-screen button on managed window
     */
    bool                     dirtyMainToolBar;

    /**
     * Store previous visibility of toolbars before ful-screen mode.
     */
    QMap<KToolBar*, bool>    toolbarsVisibility;

    /**
     * Store previous visibility of thumbbar before ful-screen mode.
     */
    bool                     thumbbarVisibility;

    /**
     * Store previous visibility of menubar before ful-screen mode.
     */
    bool                     menubarVisibility;

    /**
     * Store previous visibility of statusbar before ful-screen mode.
     */
    bool                     statusbarVisibility;

    // Common Help actions
    QAction*                 dbStatAction;
    QAction*                 libsInfoAction;
    QAction*                 showMenuBarAction;
    QAction*                 showStatusBarAction;
    DAboutData*              about;
    DLogoAction*             anim;

    QString                  configGroupName;
};

// --------------------------------------------------------------------------------------------------

DXmlGuiWindow::DXmlGuiWindow(QWidget* const parent, Qt::WindowFlags f)
    : KXmlGuiWindow(parent, f),
      d(new Private)
{
    m_expoBlendingAction       = 0;
    m_panoramaAction           = 0;
    m_videoslideshowAction     = 0;
    m_htmlGalleryAction        = 0;
    m_jalbumAction             = 0;
    m_sendByMailAction         = 0;
    m_printCreatorAction       = 0;
    m_calendarAction           = 0;
    m_presentationAction       = 0;
    m_metadataEditAction       = 0;
    m_geolocationEditAction    = 0;
    m_mediaServerAction        = 0;
    m_timeAdjustAction         = 0;
    m_animLogo                 = 0;

    // Export tools

    m_exportDropboxAction      = 0;
    m_exportOnedriveAction     = 0;
    m_exportPinterestAction    = 0;
    m_exportBoxAction          = 0;
    m_exportFacebookAction     = 0;
    m_exportFlickrAction       = 0;
    m_exportGdriveAction       = 0;
    m_exportGphotoAction       = 0;
    m_exportImageshackAction   = 0;
    m_exportImgurAction        = 0;
    m_exportIpfsAction         = 0;
    m_exportPiwigoAction       = 0;
    m_exportRajceAction        = 0;
    m_exportSmugmugAction      = 0;
    m_exportYandexfotkiAction  = 0;
    m_exportMediawikiAction    = 0;

#ifdef HAVE_VKONTAKTE
    m_exportVkontakteAction    = 0;
#endif

#ifdef HAVE_KIO
    m_exportFileTransferAction = 0;
#endif

    // Import tools

    m_importGphotoAction       = 0;
    m_importSmugmugAction      = 0;

#ifdef HAVE_KIO
    m_importFileTransferAction = 0;
#endif

#ifdef HAVE_KSANE
    m_digitalScannerAction     = 0;
#endif

    installEventFilter(this);
}

DXmlGuiWindow::~DXmlGuiWindow()
{
    delete d;
}

void DXmlGuiWindow::setConfigGroupName(const QString& name)
{
    d->configGroupName = name;
}

QString DXmlGuiWindow::configGroupName() const
{
    return d->configGroupName;
}

void DXmlGuiWindow::closeEvent(QCloseEvent* e)
{
    if (fullScreenIsActive())
        slotToggleFullScreen(false);

    if (!testAttribute(Qt::WA_DeleteOnClose))
    {
        setVisible(false);
        e->ignore();
        return;
    }

    KXmlGuiWindow::closeEvent(e);
    e->accept();
}

void DXmlGuiWindow::setFullScreenOptions(int options)
{
    d->fsOptions = options;
}

void DXmlGuiWindow::registerPluginsActions()
{
    DPluginLoader* const dpl = DPluginLoader::instance();
    dpl->registerPlugins(this);

    m_sendByMailAction = DPluginLoader::instance()->pluginAction(QLatin1String("sendbymail"), this);
    actionCollection()->addActions(QList<QAction*>() << m_sendByMailAction);

    m_printCreatorAction = DPluginLoader::instance()->pluginAction(QLatin1String("printcreator"), this);
    actionCollection()->addActions(QList<QAction*>() << m_printCreatorAction);

#ifdef HAVE_HTMLGALLERY
    m_htmlGalleryAction = DPluginLoader::instance()->pluginAction(QLatin1String("htmlgallery"), this);
    actionCollection()->addActions(QList<QAction*>() << m_htmlGalleryAction);
#endif

#ifdef HAVE_MEDIAPLAYER
    m_videoslideshowAction = DPluginLoader::instance()->pluginAction(QLatin1String("videoslideshow"), this);
    actionCollection()->addActions(QList<QAction*>() << m_videoslideshowAction);
#endif

    m_calendarAction = DPluginLoader::instance()->pluginAction(QLatin1String("calendar"), this);
    actionCollection()->addActions(QList<QAction*>() << m_calendarAction);

    m_expoBlendingAction = DPluginLoader::instance()->pluginAction(QLatin1String("expoblending"), this);
    actionCollection()->addActions(QList<QAction*>() << m_expoBlendingAction);

#ifdef HAVE_PANORAMA
    m_panoramaAction = DPluginLoader::instance()->pluginAction(QLatin1String("panorama"), this);
    actionCollection()->addActions(QList<QAction*>() << m_panoramaAction);
#endif

    m_jalbumAction = DPluginLoader::instance()->pluginAction(QLatin1String("jalbum"), this);
    actionCollection()->addActions(QList<QAction*>() << m_jalbumAction);

    m_mediaServerAction = DPluginLoader::instance()->pluginAction(QLatin1String("mediaserver"), this);
    actionCollection()->addActions(QList<QAction*>() << m_mediaServerAction);

    m_timeAdjustAction = DPluginLoader::instance()->pluginAction(QLatin1String("timeadjust_edit"), this);
    actionCollection()->addActions(QList<QAction*>() << m_timeAdjustAction);

    m_metadataEditAction = DPluginLoader::instance()->pluginAction(QLatin1String("metadata_edit"), this);
    actionCollection()->addActions(QList<QAction*>() << m_metadataEditAction);

#ifdef HAVE_MARBLE
    m_geolocationEditAction = DPluginLoader::instance()->pluginAction(QLatin1String("geolocation_edit"), this);
    actionCollection()->addActions(QList<QAction*>() << m_geolocationEditAction);
#endif

#ifdef HAVE_KSANE
    m_digitalScannerAction = DPluginLoader::instance()->pluginAction(QLatin1String("import_scan"), this);
    actionCollection()->addActions(QList<QAction*>() << m_digitalScannerAction);
#endif

    m_presentationAction = DPluginLoader::instance()->pluginAction(QLatin1String("presentation"), this);
    actionCollection()->addActions(QList<QAction*>() << m_presentationAction);

    m_exportImgurAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_imgur"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportImgurAction);

    m_exportIpfsAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_ipfs"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportIpfsAction);

    m_exportBoxAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_box"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportBoxAction);

    m_exportDropboxAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_dropbox"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportDropboxAction);

    m_exportOnedriveAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_onedrive"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportOnedriveAction);

    m_exportPinterestAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_pinterest"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportPinterestAction);

    m_exportMediawikiAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_MediaWiki"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportMediawikiAction);

    m_exportImageshackAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_imageshack"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportImageshackAction);

    m_exportPiwigoAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_piwigo"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportPiwigoAction);

    m_exportRajceAction = DPluginLoader::instance()->pluginAction(QLatin1String("export_rajce"), this);
    actionCollection()->addActions(QList<QAction*>() << m_exportRajceAction);
}

void DXmlGuiWindow::createHelpActions(bool coreOptions)
{
    d->libsInfoAction = new QAction(QIcon::fromTheme(QLatin1String("help-about")), i18n("Components Information"), this);
    connect(d->libsInfoAction, SIGNAL(triggered()), this, SLOT(slotComponentsInfo()));
    actionCollection()->addAction(QLatin1String("help_librariesinfo"), d->libsInfoAction);

    d->about          = new DAboutData(this);

    QAction* const rawCameraListAction = new QAction(QIcon::fromTheme(QLatin1String("image-x-adobe-dng")), i18n("Supported RAW Cameras"), this);
    connect(rawCameraListAction, SIGNAL(triggered()), this, SLOT(slotRawCameraList()));
    actionCollection()->addAction(QLatin1String("help_rawcameralist"), rawCameraListAction);

    QAction* const donateMoneyAction   = new QAction(QIcon::fromTheme(QLatin1String("globe")), i18n("Donate..."), this);
    connect(donateMoneyAction, SIGNAL(triggered()), this, SLOT(slotDonateMoney()));
    actionCollection()->addAction(QLatin1String("help_donatemoney"), donateMoneyAction);

    QAction* const recipesBookAction   = new QAction(QIcon::fromTheme(QLatin1String("globe")), i18n("Recipes Book..."), this);
    connect(recipesBookAction, SIGNAL(triggered()), this, SLOT(slotRecipesBook()));
    actionCollection()->addAction(QLatin1String("help_recipesbook"), recipesBookAction);

    QAction* const contributeAction    = new QAction(QIcon::fromTheme(QLatin1String("globe")), i18n("Contribute..."), this);
    connect(contributeAction, SIGNAL(triggered()), this, SLOT(slotContribute()));
    actionCollection()->addAction(QLatin1String("help_contribute"), contributeAction);

    QAction* const helpAction          = new QAction(QIcon::fromTheme(QLatin1String("help-contents")), i18n("Online Handbook..."), this);
    connect(helpAction, SIGNAL(triggered()), this, SLOT(slotHelpContents()));
    actionCollection()->addAction(QLatin1String("help_handbook"), helpAction);

    m_animLogo = new DLogoAction(this);
    actionCollection()->addAction(QLatin1String("logo_action"), m_animLogo);

    // Add options only for core components (typically all excepted Showfoto)
    if (coreOptions)
    {
        d->dbStatAction = new QAction(QIcon::fromTheme(QLatin1String("network-server-database")), i18n("Database Statistics"), this);
        connect(d->dbStatAction, SIGNAL(triggered()), this, SLOT(slotDBStat()));
        actionCollection()->addAction(QLatin1String("help_dbstat"), d->dbStatAction);
    }
}

void DXmlGuiWindow::cleanupActions()
{
    QAction* ac = actionCollection()->action(QLatin1String("help_about_kde"));
    if (ac) actionCollection()->removeAction(ac);

    ac          = actionCollection()->action(QLatin1String("help_donate"));
    if (ac) actionCollection()->removeAction(ac);

    ac          = actionCollection()->action(QLatin1String("help_contents"));
    if (ac) actionCollection()->removeAction(ac);

/*
    foreach (QAction* const act, actionCollection()->actions())
        qCDebug(DIGIKAM_WIDGETS_LOG) << "action: " << act->objectName();
*/
}

void DXmlGuiWindow::createSidebarActions()
{
    KActionCollection* const ac = actionCollection();
    QAction* const tlsb         = new QAction(i18n("Toggle Left Side-bar"), this);
    connect(tlsb, SIGNAL(triggered()), this, SLOT(slotToggleLeftSideBar()));
    ac->addAction(QLatin1String("toggle-left-sidebar"), tlsb);
    ac->setDefaultShortcut(tlsb, Qt::CTRL + Qt::META + Qt::Key_Left);

    QAction* const trsb = new QAction(i18n("Toggle Right Side-bar"), this);
    connect(trsb, SIGNAL(triggered()), this, SLOT(slotToggleRightSideBar()));
    ac->addAction(QLatin1String("toggle-right-sidebar"), trsb);
    ac->setDefaultShortcut(trsb, Qt::CTRL + Qt::META + Qt::Key_Right);

    QAction* const plsb = new QAction(i18n("Previous Left Side-bar Tab"), this);
    connect(plsb, SIGNAL(triggered()), this, SLOT(slotPreviousLeftSideBarTab()));
    ac->addAction(QLatin1String("previous-left-sidebar-tab"), plsb);
    ac->setDefaultShortcut(plsb, Qt::CTRL + Qt::META + Qt::Key_Home);

    QAction* const nlsb = new QAction(i18n("Next Left Side-bar Tab"), this);
    connect(nlsb, SIGNAL(triggered()), this, SLOT(slotNextLeftSideBarTab()));
    ac->addAction(QLatin1String("next-left-sidebar-tab"), nlsb);
    ac->setDefaultShortcut(nlsb, Qt::CTRL + Qt::META + Qt::Key_End);

    QAction* const prsb = new QAction(i18n("Previous Right Side-bar Tab"), this);
    connect(prsb, SIGNAL(triggered()), this, SLOT(slotPreviousRightSideBarTab()));
    ac->addAction(QLatin1String("previous-right-sidebar-tab"), prsb);
    ac->setDefaultShortcut(prsb, Qt::CTRL + Qt::META + Qt::Key_PageUp);

    QAction* const nrsb = new QAction(i18n("Next Right Side-bar Tab"), this);
    connect(nrsb, SIGNAL(triggered()), this, SLOT(slotNextRightSideBarTab()));
    ac->addAction(QLatin1String("next-right-sidebar-tab"), nrsb);
    ac->setDefaultShortcut(nrsb, Qt::CTRL + Qt::META + Qt::Key_PageDown);
}

void DXmlGuiWindow::createSettingsActions()
{
    d->showMenuBarAction   = KStandardAction::showMenubar(this, SLOT(slotShowMenuBar()), actionCollection());
#ifdef Q_OS_OSX
    // Under MacOS the menu bar visibility is managed by desktop.
    d->showMenuBarAction->setVisible(false);
#endif

    d->showStatusBarAction = actionCollection()->action(QLatin1String("options_show_statusbar"));

    if (!d->showStatusBarAction)
    {
        qCWarning(DIGIKAM_WIDGETS_LOG) << "Status bar menu action cannot be found in action collection";

        d->showStatusBarAction = new QAction(i18n("Show Statusbar"), this);
        d->showStatusBarAction->setCheckable(true);
        d->showStatusBarAction->setChecked(true);
        connect(d->showStatusBarAction, SIGNAL(toggled(bool)), this, SLOT(slotShowStatusBar()));
        actionCollection()->addAction(QLatin1String("options_show_statusbar"), d->showStatusBarAction);
    }

    KStandardAction::keyBindings(this,            SLOT(slotEditKeys()),          actionCollection());
    KStandardAction::preferences(this,            SLOT(slotSetup()),             actionCollection());
    KStandardAction::configureToolbars(this,      SLOT(slotConfToolbars()),      actionCollection());

#ifdef HAVE_KNOTIFYCONFIG
    KStandardAction::configureNotifications(this, SLOT(slotConfNotifications()), actionCollection());
#endif
}

QAction* DXmlGuiWindow::showMenuBarAction() const
{
    return d->showMenuBarAction;
}

QAction* DXmlGuiWindow::showStatusBarAction() const
{
    return d->showStatusBarAction;
}

void DXmlGuiWindow::slotShowMenuBar()
{
    menuBar()->setVisible(d->showMenuBarAction->isChecked());
}

void DXmlGuiWindow::slotShowStatusBar()
{
    statusBar()->setVisible(d->showStatusBarAction->isChecked());
}

void DXmlGuiWindow::slotConfNotifications()
{
#ifdef HAVE_KNOTIFYCONFIG
    KNotifyConfigWidget::configure(this);
#endif
}

void DXmlGuiWindow::editKeyboardShortcuts(KActionCollection* const extraac, const QString& actitle)
{
    KShortcutsDialog dialog(KShortcutsEditor::AllActions,
                            KShortcutsEditor::LetterShortcutsAllowed, this);
    dialog.addCollection(actionCollection(), i18nc("general keyboard shortcuts", "General"));

    if (extraac)
        dialog.addCollection(extraac, actitle);

    dialog.configure();
}

void DXmlGuiWindow::slotConfToolbars()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(configGroupName());
    saveMainWindowSettings(group);

    KEditToolBar dlg(factory(), this);

    connect(&dlg, SIGNAL(newToolbarConfig()),
            this, SLOT(slotNewToolbarConfig()));

    dlg.exec();
}

void DXmlGuiWindow::slotNewToolbarConfig()
{
    KConfigGroup group = KSharedConfig::openConfig()->group(configGroupName());
    applyMainWindowSettings(group);
}

void DXmlGuiWindow::createFullScreenAction(const QString& name)
{
    d->fullScreenAction = KStandardAction::fullScreen(0, 0, this, this);
    actionCollection()->addAction(name, d->fullScreenAction);
    d->fullScreenBtn    = new QToolButton(this);
    d->fullScreenBtn->setDefaultAction(d->fullScreenAction);
    d->fullScreenBtn->hide();

    connect(d->fullScreenAction, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleFullScreen(bool)));
}

void DXmlGuiWindow::readFullScreenSettings(const KConfigGroup& group)
{
    if (d->fsOptions & FS_TOOLBARS)
        d->fullScreenHideToolBars  = group.readEntry(s_configFullScreenHideToolBarsEntry,  false);

    if (d->fsOptions & FS_THUMBBAR)
        d->fullScreenHideThumbBar = group.readEntry(s_configFullScreenHideThumbBarEntry, true);

    if (d->fsOptions & FS_SIDEBARS)
        d->fullScreenHideSideBars  = group.readEntry(s_configFullScreenHideSideBarsEntry,  false);
}

void DXmlGuiWindow::slotToggleFullScreen(bool set)
{
    KToggleFullScreenAction::setFullScreen(this, set);

    customizedFullScreenMode(set);

    if (!set)
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "TURN OFF fullscreen";

        // restore menubar

        if (d->menubarVisibility)
            menuBar()->setVisible(true);

        // restore statusbar

        if (d->statusbarVisibility)
            statusBar()->setVisible(true);

        // restore sidebars

        if ((d->fsOptions & FS_SIDEBARS) && d->fullScreenHideSideBars)
            showSideBars(true);

        // restore thumbbar

        if ((d->fsOptions & FS_THUMBBAR) && d->fullScreenHideThumbBar)
            showThumbBar(d->thumbbarVisibility);

        // restore toolbars and manage full-screen button

        showToolBars(true);
        d->fullScreenBtn->hide();

        if (d->dirtyMainToolBar)
        {
            KToolBar* const mainbar = mainToolBar();

            if (mainbar)
            {
                mainbar->removeAction(d->fullScreenAction);
            }
        }
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "TURN ON fullscreen";

        // hide menubar

#ifdef Q_OS_WIN
        d->menubarVisibility = d->showMenuBarAction->isChecked();
#else
        d->menubarVisibility = menuBar()->isVisible();
#endif
        menuBar()->setVisible(false);

        // hide statusbar

#ifdef Q_OS_WIN
        d->statusbarVisibility = d->showStatusBarAction->isChecked();
#else
        d->statusbarVisibility = statusBar()->isVisible();
#endif
        statusBar()->setVisible(false);

        // hide sidebars

        if ((d->fsOptions & FS_SIDEBARS) && d->fullScreenHideSideBars)
            showSideBars(false);

        // hide thumbbar

        d->thumbbarVisibility = thumbbarVisibility();

        if ((d->fsOptions & FS_THUMBBAR) && d->fullScreenHideThumbBar)
            showThumbBar(false);

        // hide toolbars and manage full-screen button

        if ((d->fsOptions & FS_TOOLBARS) && d->fullScreenHideToolBars)
        {
            showToolBars(false);
        }
        else
        {
            showToolBars(true);

            // add fullscreen action if necessary in toolbar

            KToolBar* const mainbar = mainToolBar();

            if (mainbar && !mainbar->actions().contains(d->fullScreenAction))
            {
                if (mainbar->actions().isEmpty())
                {
                    mainbar->addAction(d->fullScreenAction);
                }
                else
                {
                    mainbar->insertAction(mainbar->actions().first(), d->fullScreenAction);
                }

                d->dirtyMainToolBar = true;
            }
            else
            {
                // If FullScreen button is enabled in toolbar settings,
                // we shall not remove it when leaving of fullscreen mode.
                d->dirtyMainToolBar = false;
            }
        }
    }
}

bool DXmlGuiWindow::fullScreenIsActive() const
{
    if (d->fullScreenAction)
        return d->fullScreenAction->isChecked();

    qCDebug(DIGIKAM_WIDGETS_LOG) << "FullScreenAction is not initialized";
    return false;
}

bool DXmlGuiWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == this)
    {
        if (ev && (ev->type() == QEvent::HoverMove) && fullScreenIsActive())
        {
            // We will handle a stand alone FullScreen button action on top/right corner of screen
            // only if managed window tool bar is hidden, and if we switched already in Full Screen mode.

            KToolBar* const mainbar = mainToolBar();

            if (mainbar)
            {
                if (((d->fsOptions & FS_TOOLBARS) && d->fullScreenHideToolBars) || !mainbar->isVisible())
                {
                    QHoverEvent* const mev = dynamic_cast<QHoverEvent*>(ev);

                    if (mev)
                    {
                        QPoint pos(mev->pos());
                        QRect  desktopRect = QApplication::desktop()->screenGeometry(this);

                        QRect sizeRect(QPoint(0, 0), d->fullScreenBtn->size());
                        QRect topLeft, topRight;
                        QRect topRightLarger;

                        desktopRect        = QRect(desktopRect.y(), desktopRect.y(), desktopRect.width(), desktopRect.height());
                        topLeft            = sizeRect;
                        topRight           = sizeRect;

                        topLeft.moveTo(desktopRect.x(), desktopRect.y());
                        topRight.moveTo(desktopRect.x() + desktopRect.width() - sizeRect.width() - 1, topLeft.y());

                        topRightLarger     = topRight.adjusted(-25, 0, 0, 10);

                        if (topRightLarger.contains(pos))
                        {
                            d->fullScreenBtn->move(topRight.topLeft());
                            d->fullScreenBtn->show();
                        }
                        else
                        {
                            d->fullScreenBtn->hide();
                        }

                        return false;
                    }
                }
            }
        }
    }

    // pass the event on to the parent class
    return QObject::eventFilter(obj, ev);
}

void DXmlGuiWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        if (fullScreenIsActive())
        {
            d->fullScreenAction->activate(QAction::Trigger);
        }
    }
}

KToolBar* DXmlGuiWindow::mainToolBar() const
{
    QList<KToolBar*> toolbars = toolBars();
    KToolBar* mainToolbar     = 0;

    foreach (KToolBar* const toolbar, toolbars)
    {
        if (toolbar && (toolbar->objectName() == QLatin1String("mainToolBar")))
        {
            mainToolbar = toolbar;
            break;
        }
    }

    return mainToolbar;
}

void DXmlGuiWindow::showToolBars(bool visible)
{
    // We will hide toolbars: store previous state for future restoring.
    if (!visible)
    {
        d->toolbarsVisibility.clear();

        foreach (KToolBar* const toolbar, toolBars())
        {
            if (toolbar)
            {
                bool visibility = toolbar->isVisible();
                d->toolbarsVisibility.insert(toolbar, visibility);
            }
        }
    }

    // Switch toolbars visibility
    for (QMap<KToolBar*, bool>::const_iterator it = d->toolbarsVisibility.constBegin(); it != d->toolbarsVisibility.constEnd(); ++it)
    {
        KToolBar* const toolbar = it.key();
        bool visibility         = it.value();

        if (toolbar)
        {
            if (visible && visibility)
                toolbar->show();
            else
                toolbar->hide();
        }
    }

    // We will show toolbars: restore previous state.
    if (visible)
    {
        for (QMap<KToolBar*, bool>::const_iterator it = d->toolbarsVisibility.constBegin(); it != d->toolbarsVisibility.constEnd(); ++it)
        {
            KToolBar* const toolbar = it.key();
            bool visibility         = it.value();

            if (toolbar)
            {
                visibility ? toolbar->show() : toolbar->hide();
            }
        }
    }
}

void DXmlGuiWindow::showSideBars(bool visible)
{
    Q_UNUSED(visible);
}

void DXmlGuiWindow::showThumbBar(bool visible)
{
    Q_UNUSED(visible);
}

void DXmlGuiWindow::customizedFullScreenMode(bool set)
{
    Q_UNUSED(set);
}

bool DXmlGuiWindow::thumbbarVisibility() const
{
    return true;
}

void DXmlGuiWindow::slotHelpContents()
{
    openHandbook();
}

void DXmlGuiWindow::openHandbook()
{
    QUrl url = QUrl(QString::fromUtf8("https://docs.kde.org/trunk5/en/extragear-graphics/%1/index.html")
               .arg(QApplication::applicationName()));

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::restoreWindowSize(QWindow* const win, const KConfigGroup& group)
{
    KWindowConfig::restoreWindowSize(win, group);
}

void DXmlGuiWindow::saveWindowSize(QWindow* const win, KConfigGroup& group)
{
    KWindowConfig::saveWindowSize(win, group);
}

QAction* DXmlGuiWindow::buildStdAction(StdActionType type, const QObject* const recvr,
                                       const char* const slot, QObject* const parent)
{
    switch(type)
    {
        case StdCopyAction:
            return KStandardAction::copy(recvr, slot, parent);
            break;
        case StdPasteAction:
            return KStandardAction::paste(recvr, slot, parent);
            break;
        case StdCutAction:
            return KStandardAction::cut(recvr, slot, parent);
            break;
        case StdQuitAction:
            return KStandardAction::quit(recvr, slot, parent);
            break;
        case StdCloseAction:
            return KStandardAction::close(recvr, slot, parent);
            break;
        case StdZoomInAction:
            return KStandardAction::zoomIn(recvr, slot, parent);
            break;
        case StdZoomOutAction:
            return KStandardAction::zoomOut(recvr, slot, parent);
            break;
        case StdOpenAction:
#ifndef __clang_analyzer__
            // NOTE: disable false positive report from scan build about open()
            return KStandardAction::open(recvr, slot, parent);
#endif
            break;
        case StdSaveAction:
            return KStandardAction::save(recvr, slot, parent);
            break;
        case StdSaveAsAction:
            return KStandardAction::saveAs(recvr, slot, parent);
            break;
        case StdRevertAction:
            return KStandardAction::revert(recvr, slot, parent);
            break;
        case StdBackAction:
            return KStandardAction::back(recvr, slot, parent);
            break;
        case StdForwardAction:
            return KStandardAction::forward(recvr, slot, parent);
            break;
        default:
            return 0;
            break;
    }
}

void DXmlGuiWindow::slotRawCameraList()
{
    showRawCameraList();
}

void DXmlGuiWindow::slotDonateMoney()
{
    WebBrowserDlg* const browser
        = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/donate/")),
                            qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::slotRecipesBook()
{
    WebBrowserDlg* const browser
        = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/recipes_book/")),
                            qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::slotContribute()
{
    WebBrowserDlg* const browser
        = new WebBrowserDlg(QUrl(QLatin1String("https://www.digikam.org/contribute/")),
                            qApp->activeWindow());
    browser->show();
}

void DXmlGuiWindow::setupIconTheme()
{
    // Let QStandardPaths handle this, it will look for app local stuff
    // this means e.g. for mac: "<APPDIR>/../Resources" and for win: "<APPDIR>/data".

    bool hasBreeze                = false;
    const QString breezeIcons     = QStandardPaths::locate(QStandardPaths::DataLocation, QLatin1String("breeze.rcc"));

    if (!breezeIcons.isEmpty() && QFile::exists(breezeIcons))
    {
        QResource::registerResource(breezeIcons);
        hasBreeze = true;
    }

    bool hasBreezeDark            = false;
    const QString breezeDarkIcons = QStandardPaths::locate(QStandardPaths::DataLocation, QLatin1String("breeze-dark.rcc"));

    if (!breezeDarkIcons.isEmpty() && QFile::exists(breezeDarkIcons))
    {
        QResource::registerResource(breezeDarkIcons);
        hasBreezeDark = true;
    }

    if (hasBreeze || hasBreezeDark)
    {
        // Tell Qt about the theme
        QIcon::setThemeSearchPaths(QStringList() << QLatin1String(":/icons"));

        // Tell icons loader an co. about the theme
        KConfigGroup cg(KSharedConfig::openConfig(), "Icons");

        if (hasBreeze)
        {
            QIcon::setThemeName(QLatin1String("breeze"));
            cg.writeEntry("Theme", "breeze");
            qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze icons resource file found";
        }
        else if (hasBreezeDark)
        {
            QIcon::setThemeName(QLatin1String("breeze-dark"));
            cg.writeEntry("Theme", "breeze-dark");
            qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze-dark icons resource file found";
        }
        else
        {
            qCDebug(DIGIKAM_WIDGETS_LOG) << "No icons resource file found";
        }

        cg.sync();
    }
}

void DXmlGuiWindow::createExportActions()
{
    m_exportFacebookAction = new QAction(i18n("Export to &Facebook..."), this);
    m_exportFacebookAction->setIcon(QIcon::fromTheme(QString::fromLatin1("dk-facebook")));
    actionCollection()->addAction(QLatin1String("export_facebook"), m_exportFacebookAction);
    actionCollection()->setDefaultShortcut(m_exportFacebookAction, Qt::ALT + Qt::SHIFT + Qt::Key_F);

    connect(m_exportFacebookAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

    m_exportFlickrAction = new QAction(i18n("Export to Flick&r..."), this);
    m_exportFlickrAction->setIcon(QIcon::fromTheme(QLatin1String("dk-flickr")));
    actionCollection()->addAction(QLatin1String("export_flickr"), m_exportFlickrAction);
    actionCollection()->setDefaultShortcut(m_exportFlickrAction, Qt::ALT + Qt::SHIFT + Qt::Key_R);

    connect(m_exportFlickrAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

    m_exportGdriveAction = new QAction(i18n("Export to &Google Drive..."), this);
    m_exportGdriveAction->setIcon(QIcon::fromTheme(QLatin1String("dk-googledrive")));
    actionCollection()->addAction(QLatin1String("export_googledrive"), m_exportGdriveAction);
    actionCollection()->setDefaultShortcut(m_exportGdriveAction, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_G);

    connect(m_exportGdriveAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

    m_exportGphotoAction = new QAction(i18n("Export to &Google Photos..."), this);
    m_exportGphotoAction->setIcon(QIcon::fromTheme(QLatin1String("dk-googlephoto")));
    actionCollection()->addAction(QLatin1String("export_googlephoto"), m_exportGphotoAction);
    actionCollection()->setDefaultShortcut(m_exportGphotoAction, Qt::ALT + Qt::SHIFT + Qt::Key_P);

    connect(m_exportGphotoAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

    m_exportSmugmugAction = new QAction(i18n("Export to &SmugMug..."), this);
    m_exportSmugmugAction->setIcon(QIcon::fromTheme(QLatin1String("dk-smugmug")));
    actionCollection()->addAction(QLatin1String("export_smugmug"), m_exportSmugmugAction);
    actionCollection()->setDefaultShortcut(m_exportSmugmugAction, Qt::ALT + Qt::SHIFT +  Qt::Key_S);

    connect(m_exportSmugmugAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

    m_exportYandexfotkiAction = new QAction(i18n("Export to &Yandex.Fotki..."), this);
    m_exportYandexfotkiAction->setIcon(QIcon::fromTheme(QLatin1String("internet-web-browser")));
    actionCollection()->addAction(QLatin1String("export_yandexfotki"), m_exportYandexfotkiAction);
    actionCollection()->setDefaultShortcut(m_exportYandexfotkiAction, Qt::ALT + Qt::SHIFT + Qt::Key_Y);

    connect(m_exportYandexfotkiAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));

#ifdef HAVE_VKONTAKTE
    m_exportVkontakteAction = new QAction(i18n("Export to &VKontakte..."), this);
    m_exportVkontakteAction->setIcon(QIcon::fromTheme(QLatin1String("preferences-web-browser-shortcuts")));
    actionCollection()->addAction(QLatin1String("export_vkontakte"), m_exportVkontakteAction);

    connect(m_exportVkontakteAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));
#endif

#ifdef HAVE_KIO
    m_exportFileTransferAction = new QAction(i18n("Export to remote storage..."), this);
    m_exportFileTransferAction->setIcon(QIcon::fromTheme(QLatin1String("folder-html")));
    actionCollection()->addAction(QLatin1String("export_filetransfer"), m_exportFileTransferAction);
    actionCollection()->setDefaultShortcut(m_exportYandexfotkiAction, Qt::ALT + Qt::SHIFT + Qt::Key_K);

    connect(m_exportFileTransferAction, SIGNAL(triggered(bool)),
            this, SLOT(slotExportTool()));
#endif
}

void DXmlGuiWindow::createImportActions()
{
    m_importGphotoAction = new QAction(i18n("Import from &Google Photos..."), this);
    m_importGphotoAction->setIcon(QIcon::fromTheme(QLatin1String("dk-googlephoto")));
    actionCollection()->addAction(QLatin1String("import_googlephoto"), m_importGphotoAction);
    actionCollection()->setDefaultShortcut(m_importGphotoAction, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_P);

    connect(m_importGphotoAction, SIGNAL(triggered(bool)),
            this, SLOT(slotImportTool()));

    m_importSmugmugAction = new QAction(i18n("Import from &SmugMug..."), this);
    m_importSmugmugAction->setIcon(QIcon::fromTheme(QLatin1String("dk-smugmug")));
    actionCollection()->addAction(QLatin1String("import_smugmug"), m_importSmugmugAction);
    actionCollection()->setDefaultShortcut(m_importSmugmugAction, Qt::ALT + Qt::SHIFT + Qt::CTRL + Qt::Key_S);

    connect(m_importSmugmugAction, SIGNAL(triggered(bool)),
            this, SLOT(slotImportTool()));

#ifdef HAVE_KIO
    m_importFileTransferAction = new QAction(i18n("Import from remote storage..."), this);
    m_importFileTransferAction->setIcon(QIcon::fromTheme(QLatin1String("folder-html")));
    actionCollection()->addAction(QLatin1String("import_filetransfer"), m_importFileTransferAction);
    actionCollection()->setDefaultShortcut(m_importFileTransferAction, Qt::ALT + Qt::SHIFT + Qt::Key_I);

    connect(m_importFileTransferAction, SIGNAL(triggered(bool)),
            this, SLOT(slotImportTool()));
#endif
}

QList<QAction*> DXmlGuiWindow::exportActions() const
{
    return QList<QAction*>() << m_exportDropboxAction
                             << m_exportOnedriveAction
                             << m_exportPinterestAction
                             << m_exportBoxAction
                             << m_exportFacebookAction
                             << m_exportFlickrAction
                             << m_exportGdriveAction
                             << m_exportGphotoAction
                             << m_exportImageshackAction
                             << m_exportImgurAction
                             << m_exportIpfsAction
                             << m_exportPiwigoAction
                             << m_exportRajceAction
                             << m_exportSmugmugAction
                             << m_exportYandexfotkiAction
                             << m_exportMediawikiAction

#ifdef HAVE_VKONTAKTE
                             << m_exportVkontakteAction
#endif

#ifdef HAVE_KIO
                             << m_exportFileTransferAction;
#endif
                             ;
}

QList<QAction*> DXmlGuiWindow::importActions() const
{
    return QList<QAction*>() << m_importGphotoAction
                             << m_importSmugmugAction

#ifdef HAVE_KIO
                             << m_importFileTransferAction
#endif

#ifdef HAVE_KSANE
                             << m_digitalScannerAction
#endif
                             ;
}

int DXmlGuiWindow::actionToWebService(QAction* const action) const
{
    if (action == m_exportFacebookAction)
    {
        return WSStarter::ExportFacebook;
    }

#ifdef HAVE_KIO
    else if (action == m_exportFileTransferAction)
    {
        return WSStarter::ExportFileTransfer;
    }
#endif

    else if (action == m_exportFlickrAction)
    {
        return WSStarter::ExportFlickr;
    }
    else if (action == m_exportGdriveAction)
    {
        return WSStarter::ExportGdrive;
    }
    else if (action == m_exportGphotoAction)
    {
        return WSStarter::ExportGphoto;
    }
    else if (action == m_exportSmugmugAction)
    {
        return WSStarter::ExportSmugmug;
    }

#ifdef HAVE_VKONTAKTE
    else if (action == m_exportVkontakteAction)
    {
        return WSStarter::ExportVkontakte;
    }
#endif

    else if (action == m_exportYandexfotkiAction)
    {
        return WSStarter::ExportYandexfotki;
    }
    else if (action == m_importGphotoAction)
    {
        return WSStarter::ImportGphoto;
    }

#ifdef HAVE_KIO
    else if (action == m_importFileTransferAction)
    {
        return WSStarter::ImportFileTransfer;
    }
#endif

    else if (action == m_importSmugmugAction)
    {
        return WSStarter::ImportSmugmug;
    }

    return WSStarter::ExportUnknown;
}

} // namespace Digikam
