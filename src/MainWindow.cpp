#include "MainWindow.h"
#include "ui_MainWindow.h"

/*****************************************************************************/
/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);
    myLanguageModel = new MyLanguageModel(this);
    myProfile = QWebEngineProfile::defaultProfile();
    ui->setupUi(this);
    // Read Settings and set all Defaults
    readSettings();
    // Set Tab Colors
    ui->tabWidget->tabBar()->setTabTextColor(TabBookmarks, Qt::blue);
    // This does not seem to work FIXME
    ui->tabWidget->setTabShape(QTabWidget::Triangular);

    // Create Bookmark Menu
    myBookmarkMenuWidgetView = new BookmarkMenu(ui->menuBookmarks);
    // From bookmark menu
    QObject::connect(myBookmarkMenuWidgetView, &BookmarkMenu::handleOpenBookmark, this, &MainWindow::setLineEditUrl);
    //
    myTreeWidget = new BookmarkTreeWidgetView(ui->statusbar);
    ui->verticalLayoutBookmarks->insertWidget(TabBookmarks, myTreeWidget);
    //
    myLineEditUrl = new QLineEdit(this);
    myFavAction = new QAction(this);
    myLineEditUrl->addAction(myFavAction, QLineEdit::LeadingPosition);
    myLineEditUrl->setClearButtonEnabled(true);
    ui->toolBar->addWidget(myLineEditUrl);

    // Signals
    connect(myTreeWidget,         &QTreeWidget::currentItemChanged, this, &MainWindow::onTreeViewChanged);
    connect(ui->pushButtonBookmarksAdd, &QPushButton::clicked, this, &MainWindow::onAddBookmark);
    connect(ui->pushButtonBookmarksFolder, &QPushButton::clicked, this, &MainWindow::onAddFolder);
    connect(ui->pushButtonBookmarksAdd, &QPushButton::clicked, this, &MainWindow::onAddBookmark);
    connect(ui->pushButtonBookmarksSeparator, &QPushButton::clicked, this, &MainWindow::onAddSeparator);

    connect(ui->pushButtonBookmarksSave,   &QPushButton::clicked, this, &MainWindow::onSave);
    connect(ui->pushButtonBookmarksDelete, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(ui->pushButtonBookmarksClear,  &QPushButton::clicked, this, &MainWindow::onClear);
    connect(ui->spinBoxFontPointSize, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxFontPointSizeValueChanged);
    connect(ui->fontComboBox,      &QFontComboBox::currentFontChanged, this, &MainWindow::onFontComboBoxCurrentFontChanged);
    connect(ui->checkBoxIncognito, &QCheckBox::stateChanged, this, &MainWindow::onCheckBoxIncognitoStateChanged);
    // Context Menu Right-Click
    #if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
    myTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myTreeWidget, &QWidget::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
    #endif

    ui->tabWidget->tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tabWidget->tabBar(), &QTabBar::customContextMenuRequested, this, &MainWindow::handleContextMenuRequested);
    connect(ui->tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &MainWindow::closeTab);
    connect(ui->tabWidget->tabBar(), &QTabBar::tabBarDoubleClicked, this, [this](int index) { if (index == -1) { createTab(); } });

    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setElideMode(Qt::ElideRight);


    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::handleCurrentChanged);


    connect(this, &MainWindow::titleChanged, this, &MainWindow::handleWebViewTitleChanged);
    connect(this, &MainWindow::linkHovered, [this](const QString& url) { statusBar()->showMessage(url); });
    connect(this, &MainWindow::loadProgress, this, &MainWindow::handleWebViewLoadProgress);
    connect(this, &MainWindow::webActionEnabledChanged, this, &MainWindow::handleWebActionEnabledChanged);
    connect(this, &MainWindow::urlChanged, [this](const QUrl &url) { myLineEditUrl->setText(url.toDisplayString()); });
    connect(this, &MainWindow::favIconChanged, myFavAction, &QAction::setIcon);
    connect(myLineEditUrl, &QLineEdit::returnPressed, this, [this]() { setUrl(QUrl::fromUserInput(myLineEditUrl->text())); });
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    connect(this, &MainWindow::findTextFinished, this, &MainWindow::handleFindTextFinished);
    #endif

    QAction *focusUrlLineEditAction = new QAction(this);
    addAction(focusUrlLineEditAction);
    focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () { myLineEditUrl->setFocus(Qt::ShortcutFocusReason); });

    handleWebViewTitleChanged(QString());
    //
    myDownloadManagerWidget = new DownloadManagerWidget(ui->tableWidgetDownloads, this);
    //onDownloadTab();

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onSettings);
    connect(ui->actionClose_Tab, &QAction::triggered, this, &MainWindow::onCloseTab);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::onHelp);
    connect(ui->actionBackward, &QAction::triggered, this, &MainWindow::onBackward);
    connect(ui->actionDownloads, &QAction::triggered, this, &MainWindow::onDownloads);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(ui->actionFind_Next, &QAction::triggered, this, &MainWindow::onFindNext);
    connect(ui->actionFind_Previous, &QAction::triggered, this, &MainWindow::onFindPrevious);
    connect(ui->actionForward, &QAction::triggered, this, &MainWindow::onForward);
    connect(ui->actionHide_Menu, &QAction::triggered, this, &MainWindow::onHideMenu);
    connect(ui->actionHide_Statusbar, &QAction::triggered, this, &MainWindow::onHideStatusbar);
    connect(ui->actionHide_Toolbar, &QAction::triggered, this, &MainWindow::onHideToolbar);
    connect(ui->actionHistoryViewer, &QAction::triggered, this, &MainWindow::onHistoryViewer);
    connect(ui->actionIncognito, &QAction::triggered, this, &MainWindow::onIncognito);
    connect(ui->actionManager, &QAction::triggered, this, &MainWindow::onBookmarkManager);
    connect(ui->actionNew_Tab, &QAction::triggered, this, &MainWindow::onNewTab);
    connect(ui->actionNext_Tab, &QAction::triggered, this, &MainWindow::onNextTab);
    connect(ui->actionOpen_File, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actionPrevious_Tab, &QAction::triggered, this, &MainWindow::onPreviousTab);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::onQuit);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::onReload);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::onStop);
    connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::onZoomIn);
    connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::onZoomOut);
    connect(ui->actionZoom_Reset, &QAction::triggered, this, &MainWindow::onZoomReset);
    //
    createTab();
    // Set Tabs Invisible
    ui->tabWidget->tabBar()->setTabVisible(TabBookmarks, false);
    ui->tabWidget->tabBar()->setTabVisible(TabDownload, false);
    ui->tabWidget->tabBar()->setTabVisible(TabHistory, false);
    ui->tabWidget->tabBar()->setTabVisible(TabSettings, false);
    ui->tabWidget->tabBar()->setTabVisible(TabWizard, false);

}
/*****************************************************************************/
/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}
/*****************************************************************************/
/**
 * @brief MainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
    deleteLater();
}
/*****************************************************************************/
/**
 * @brief MainWindow::sizeHint
 * @return
 */
QSize MainWindow::sizeHint() const
{
    QRect desktopRect = QApplication::primaryScreen()->geometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}
/*****************************************************************************/
/**
 * @brief MainWindow::onFind
 */
void MainWindow::onFind()
{
    if (!currentTab()) { return; }
    bool ok = false;
    // FIXME
    QString search = QInputDialog::getText(this, tr("Find"), tr("Find:"), QLineEdit::Normal, myLastSearch, &ok);
    if (ok && !search.isEmpty())
    {
        myLastSearch = search;
        #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        currentTab()->findText(myLastSearch);
        #else
        currentTab()->findText(myLastSearch, 0, [this](bool found) { if (!found) { statusBar()->showMessage(tr("\"%1\" not found.").arg(myLastSearch)); } });
        #endif
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onFindNext
 */
void MainWindow::onFindNext()
{
    if (!currentTab() || myLastSearch.isEmpty()) { return; }
    currentTab()->findText(myLastSearch);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onFindPrevious
 */
void MainWindow::onFindPrevious()
{
    if (!currentTab() || myLastSearch.isEmpty()) { return; }
    currentTab()->findText(myLastSearch, QWebEnginePage::FindBackward);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onForward
 */
void MainWindow::onForward()
{
    triggerWebPageAction(QWebEnginePage::Forward);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onBackward
 */
void MainWindow::onBackward()
{
    triggerWebPageAction(QWebEnginePage::Back);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onHideMenu
 */
void MainWindow::onHideMenu()
{
    if (ui->menubar->isVisible())
    {
        ui->menubar->setVisible(false);
    }
    else
    {
        ui->menubar->setVisible(true);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onHideStatusbar
 */
void MainWindow::onHideStatusbar()
{
    if (statusBar()->isVisible())
    {
        ui->actionHide_Statusbar->setText(tr("Show Status Bar"));
        statusBar()->close();
    }
    else
    {
        ui->actionHide_Statusbar->setText(tr("Hide Status Bar"));
        statusBar()->show();
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onHideToolbar
 */
void MainWindow::onHideToolbar()
{
    if (ui->tabWidget->tabBar()->isVisible())
    {
        ui->actionHide_Toolbar->setText(tr("Show Toolbar"));
        ui->tabWidget->tabBar()->close();
    }
    else
    {
        ui->actionHide_Toolbar->setText(tr("Hide Toolbar"));
        ui->tabWidget->tabBar()->show();
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onHistoryViewer
 */
void MainWindow::onHistoryViewer()
{
    if (ui->tabWidget->tabBar()->isTabVisible(TabHistory))
    {
        ui->tabWidget->tabBar()->setTabVisible(TabHistory, false);
    }
    else
    {
        ui->tabWidget->tabBar()->setTabVisible(TabHistory, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabHistory);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onIncognito
 */
void MainWindow::onIncognito()
{
    // FIXME
    if (ui->actionIncognito->isChecked())
    {
        ui->actionNew_Tab->setText(tr("New Incognito Tab"));
        ui->checkBoxIncognito->setChecked(true);
    }
    else
    {
        ui->actionNew_Tab->setText(tr("New Tab"));
        ui->checkBoxIncognito->setChecked(false);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::on_checkBoxIncognito_stateChanged
 * @param thisState
 */
void MainWindow::onCheckBoxIncognitoStateChanged(int thisState)
{
    if (thisState == Qt::CheckState::Checked)
    {
        ui->actionIncognito->setChecked(true);
    }
    else
    {
        ui->actionIncognito->setChecked(false);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onBookmarkManager
 */
void MainWindow::onBookmarkManager()
{

    if (ui->tabWidget->tabBar()->isTabVisible(TabBookmarks))
    {
        // FIXME
        if (ui->tabWidget->tabBar()->currentIndex() == TabBookmarks)
        {
            ui->tabWidget->tabBar()->setTabVisible(TabBookmarks, false);
        }
        else
        {
            ui->tabWidget->tabBar()->setCurrentIndex(TabBookmarks);
        }
    }
    else
    {
        ui->lineEditBookmarksUrl->setText(myLineEditUrl->text());
        ui->lineEditBookmarksTitle->setText(ui->tabWidget->tabBar()->tabText(ui->tabWidget->tabBar()->currentIndex()));

        myTreeWidget->setTitle(ui->lineEditBookmarksTitle->text());
        myTreeWidget->setUrl(ui->lineEditBookmarksUrl->text());
        ui->tabWidget->tabBar()->setTabVisible(TabBookmarks, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabBookmarks);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onNewTab
 */
void MainWindow::onNewTab()
{
    createTab();
    myLineEditUrl->setFocus();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onNextTab
 */
void MainWindow::onNextTab()
{
    nextTab();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onOpenFile
 */
void MainWindow::onOpenFile()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(), tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty()) { return; }
    currentTab()->setUrl(url);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onPreviousTab
 */
void MainWindow::onPreviousTab()
{
    previousTab();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onQuit
 */
void MainWindow::onQuit()
{
    close();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onReload
 */
void MainWindow::onReload()
{
    triggerWebPageAction(QWebEnginePage::Reload);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onStop
 */
void MainWindow::onStop()
{
    triggerWebPageAction(QWebEnginePage::Stop);
    triggerWebPageAction(QWebEnginePage::WebAction(ui->actionStop->data().toInt()));
}
/*****************************************************************************/
/**
 * @brief MainWindow::onZoomIn
 */
void MainWindow::onZoomIn()
{
    currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onZoomOut
 */
void MainWindow::onZoomOut()
{
    currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onZoomReset
 */
void MainWindow::onZoomReset()
{
    currentTab()->setZoomFactor(1.0);
}
/*****************************************************************************/
/**
 * @brief MainWindow::onSettings
 */
void MainWindow::onSettings()
{
    if (ui->tabWidget->tabBar()->isTabVisible(TabSettings))
    {
        ui->tabWidget->tabBar()->setTabVisible(TabSettings, false);
    }
    else
    {
        ui->tabWidget->tabBar()->setTabVisible(TabSettings, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabSettings);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onDownloads
 */
void MainWindow::onDownloads()
{
    if (ui->tabWidget->tabBar()->isTabVisible(TabDownload))
    {
        ui->tabWidget->tabBar()->setTabVisible(TabDownload, false);
    }
    else
    {
        ui->tabWidget->tabBar()->setTabVisible(TabDownload, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabDownload);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onHelp
 */
void MainWindow::onHelp()
{
    // FIXME file localize

    if (ui->tabWidget->tabBar()->isTabVisible(TabWizard))
    {
        if (myWizardTabIndex != WizardHelp)
        {
            myWizardTabIndex = WizardHelp;
            ui->textBrowserWizard->setMarkdown(myLanguageModel->mySetting->readFile(QString("%1_%2.md").arg(":help/Help", myLanguageModel->getLanguageCode())));
            ui->tabWidget->tabBar()->setCurrentIndex(TabWizard);
        }
        else
        {
            ui->tabWidget->tabBar()->setTabVisible(TabWizard, false);
        }
    }
    else
    {
        myWizardTabIndex = WizardHelp;
        ui->textBrowserWizard->setMarkdown(myLanguageModel->mySetting->readFile(QString("%1_%2.md").arg(":help/Help", myLanguageModel->getLanguageCode())));
        ui->tabWidget->tabBar()->setTabVisible(TabWizard, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabWizard);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onAbout
 */
void MainWindow::onAbout()
{
    // FIXME file localize
    if (ui->tabWidget->tabBar()->isTabVisible(TabWizard))
    {
        if (myWizardTabIndex != WizardAbout)
        {
            myWizardTabIndex = WizardAbout;
            ui->textBrowserWizard->setMarkdown(myLanguageModel->mySetting->readFile(QString("%1_%2.md").arg(":help/About", myLanguageModel->getLanguageCode())));
            ui->tabWidget->tabBar()->setCurrentIndex(TabWizard);
        }
        else
        {
            ui->tabWidget->tabBar()->setTabVisible(TabWizard, false);
        }
    }
    else
    {
        myWizardTabIndex = WizardAbout;
        ui->textBrowserWizard->setMarkdown(myLanguageModel->mySetting->readFile(QString("%1_%2.md").arg(":help/About", myLanguageModel->getLanguageCode())));
        ui->tabWidget->tabBar()->setTabVisible(TabWizard, true);
        ui->tabWidget->tabBar()->setCurrentIndex(TabWizard);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::onCloseTab
 */
void MainWindow::onCloseTab()
{
    ui->tabWidget->currentIndex();
}
/*****************************************************************************/
/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings()
{
    isDebugMessage = myLanguageModel->mySetting->readSettingsBool(myLanguageModel->mySetting->MY_IS_DEBUG_MESSAGE, isDebugMessage);
    // Tab Font Name
    QString theFontName = QApplication::font().family();
    myTabFontName = myLanguageModel->mySetting->readSettings(MY_FONT_NAME_TABS, theFontName);
    // Tab Font Size
    QString theFontSize = QString::number(QApplication::font().pointSize());
    myTabFontSize = myLanguageModel->mySetting->readSettings(MY_FONT_SIZE_TABS, theFontSize).toInt();
    setFontByName(myTabFontName);
    setTabFontPointSize(theFontSize.toInt());
    changeFonts();
    // Default URL
    ui->lineEditDefaultUrl->setText(myLanguageModel->mySetting->readSettings(MY_DEFAULT_URL, MY_SEND_TO_URL));
    // Incognito
    isIncognito = myLanguageModel->mySetting->readSettingsBool(MY_INCOGNITO, false);
    // FIXME do I need to set both
    ui->checkBoxIncognito->setChecked(isIncognito);
    ui->actionIncognito->setChecked(isIncognito);

    // Go to Tab
//    int theIndex = myLanguageModel->mySetting->readSettingsInt(myLanguageModel->mySetting->MY_LAST_TAB_INDEX, myLanguageModel->mySetting->MY_DEFAULT_TAB_INDEX.toInt());
//    if (theIndex < 0)
//    {
//        theIndex = 0;
//    }
//    ui->tabWidget->setCurrentIndex(theIndex);
    //resize(myMySettings->getGeometrySize());
    //move(myMySettings->getGeometryPos());
    //
    //if(myMySettings->getGeometryMax()) setWindowState(windowState() | Qt::WindowMaximized);
    //if(myMySettings->getGeometryMin()) setWindowState(windowState() | Qt::WindowMinimized);
    //
}
/*****************************************************************************/
/**
 * @brief MainWindow::writeSettings
 */
void MainWindow::writeSettings()
{
    myLanguageModel->mySetting->writeSettings(myLanguageModel->mySetting->MY_IS_DEBUG_MESSAGE, isDebugMessage ? "true" : "false");
    // Write Geometry on exit
    myLanguageModel->mySetting->setGeometry(saveGeometry());
    myLanguageModel->mySetting->setWindowState(saveState());
    // Last Tab
    myLanguageModel->mySetting->writeSettings(myLanguageModel->mySetting->MY_LAST_TAB_INDEX, QString::number(ui->tabWidget->currentIndex()));
    // Tab Font Name
    myLanguageModel->mySetting->writeSettings(MY_FONT_NAME_TABS, myTabFontName);
    // Tab Font Size
    myLanguageModel->mySetting->writeSettings(MY_FONT_SIZE_TABS, QString::number(myTabFontSize));
    // Default Url
    myLanguageModel->mySetting->writeSettings(MY_DEFAULT_URL, ui->lineEditDefaultUrl->text());
    // Incognito
    myLanguageModel->mySetting->writeSettings(MY_INCOGNITO, ui->checkBoxIncognito->isChecked() ? "true" : "false");

}
/*****************************************************************************/
/**
 * @brief MainWindow::tabWidget
 * @return
 */
QTabWidget *MainWindow::tabWidget() const
{
    return ui->tabWidget;
}
/*****************************************************************************/
/**
 * @brief MainWindow::currentTab
 * @return
 */
WebView *MainWindow::currentTab() const
{
    return currentWebView();
}
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
/*****************************************************************************/
/**
 * @brief MainWindow::handleFindTextFinished
 * @param result
 */
void MainWindow::handleFindTextFinished(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0)
    {
        statusBar()->showMessage(tr("\"%1\" not found.").arg(myLastSearch));
    }
    else
    {
        statusBar()->showMessage(tr("\"%1\" found: %2/%3").arg(myLastSearch, QString::number(result.activeMatch()), QString::number(result.numberOfMatches())));
    }
}
#endif
/*****************************************************************************/
/**
 * @brief MainWindow::handleWebViewLoadProgress
 * @param progress
 */
void MainWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":process-stop.png"));
    static QIcon reloadIcon(QStringLiteral(":view-refresh.png"));

    if (0 < progress && progress < 100)
    {
        ui->actionReload->setData(QWebEnginePage::Stop);
        ui->actionReload->setIcon(stopIcon);
        ui->actionReload->setToolTip(tr("Stop loading the current page"));
        ui->progressBar->setValue(progress);
    }
    else
    {
        ui->actionReload->setData(QWebEnginePage::Reload);
        ui->actionReload->setIcon(reloadIcon);
        ui->actionReload->setToolTip(tr("Reload the current page"));
        ui->progressBar->setValue(0);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleWebActionEnabledChanged
 * @param action
 * @param enabled
 */
void MainWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action)
    {
        case QWebEnginePage::Back:
            ui->actionBackward->setEnabled(enabled);
            break;
        case QWebEnginePage::Forward:
            ui->actionForward->setEnabled(enabled);
            break;
        case QWebEnginePage::Reload:
            ui->actionReload->setEnabled(enabled);
            break;
        case QWebEnginePage::Stop:
            ui->actionStop->setEnabled(enabled);
            break;
        default:
            qWarning("Unhandled webActionChanged signal");
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleWebViewTitleChanged
 * @param title
 */
void MainWindow::handleWebViewTitleChanged(const QString &title)
{
    QString suffix = ui->actionIncognito->isChecked() ? tr("Qt Simple Browser (Incognito)") : tr("Qt Simple Browser");
    if (title.isEmpty())
    {
        setWindowTitle(suffix);
    }
    else
    {
        setWindowTitle(title + " - " + suffix);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleFileOpenTriggered
 */
void MainWindow::handleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(), tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty()) { return; }
    currentTab()->setUrl(url);
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleFindActionTriggered
 */
void MainWindow::handleFindActionTriggered()
{
    if (!currentTab()) { return; }
    bool ok = false;
    QString search = QInputDialog::getText(this, tr("Find"), tr("Find:"), QLineEdit::Normal, myLastSearch, &ok);
    if (ok && !search.isEmpty())
    {
        myLastSearch = search;
        #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        currentTab()->findText(myLastSearch);
        #else
        currentTab()->findText(myLastSearch, 0, [this](bool found)
        {
            if (!found) { statusBar()->showMessage(tr("\"%1\" not found.").arg(myLastSearch)); }
        });
        #endif
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleContextMenuRequested
 * @param pos
 */
void MainWindow::handleContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
    menu.addAction(tr("New &Tab"), this, &MainWindow::createTab, QKeySequence::AddTab);
    int index = ui->tabWidget->tabBar()->tabAt(pos);
    if (index != -1)
    {
        QAction *action = menu.addAction(tr("Clone Tab"));
        connect(action, &QAction::triggered, this, [this,index]() { cloneTab(index); });
        menu.addSeparator();
        action = menu.addAction(tr("&Close Tab"));
        action->setShortcut(QKeySequence::Close);
        connect(action, &QAction::triggered, this, [this,index]() { closeTab(index); });
        action = menu.addAction(tr("Close &Other Tabs"));
        connect(action, &QAction::triggered, this, [this,index]() { closeOtherTabs(index); });
        menu.addSeparator();
        action = menu.addAction(tr("Reload Tab"));
        action->setShortcut(QKeySequence::Refresh);
        connect(action, &QAction::triggered, this, [this,index]() { reloadTab(index); });
    }
    else
    {
        menu.addSeparator();
    }
    menu.addAction(tr("Reload All Tabs"), this, &MainWindow::reloadAllTabs);
    menu.exec(QCursor::pos());
}
/*****************************************************************************/
/**
 * @brief MainWindow::handleCurrentChanged
 * @param index
 */
void MainWindow::handleCurrentChanged(int index)
{
    if (index != -1)
    {
        WebView *view = webView(index);
        if (!view) { return; }
        if (!view->url().isEmpty()) { view->setFocus(); }
        emit titleChanged(view->title());
        emit loadProgress(view->loadProgress());
        emit urlChanged(view->url());
        emit favIconChanged(view->favIcon());
        emit webActionEnabledChanged(QWebEnginePage::Back, view->isWebActionEnabled(QWebEnginePage::Back));
        emit webActionEnabledChanged(QWebEnginePage::Forward, view->isWebActionEnabled(QWebEnginePage::Forward));
        emit webActionEnabledChanged(QWebEnginePage::Stop, view->isWebActionEnabled(QWebEnginePage::Stop));
        emit webActionEnabledChanged(QWebEnginePage::Reload,view->isWebActionEnabled(QWebEnginePage::Reload));
    }
    else
    {
        emit titleChanged(QString());
        emit loadProgress(0);
        emit urlChanged(QUrl());
        emit favIconChanged(QIcon());
        emit webActionEnabledChanged(QWebEnginePage::Back, false);
        emit webActionEnabledChanged(QWebEnginePage::Forward, false);
        emit webActionEnabledChanged(QWebEnginePage::Stop, false);
        emit webActionEnabledChanged(QWebEnginePage::Reload, true);
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::currentWebView
 * @return
 */
WebView *MainWindow::currentWebView() const
{
    return webView(ui->tabWidget->tabBar()->currentIndex());
}
/*****************************************************************************/
/**
 * @brief MainWindow::webView
 * @param index
 * @return
 */
WebView *MainWindow::webView(int index) const
{
    return qobject_cast<WebView*>(ui->tabWidget->widget(index));
}
/*****************************************************************************/
/**
 * @brief MainWindow::setupView
 * @param webView
 */
void MainWindow::setupView(WebView *webView)
{
    QWebEnginePage *webPage = webView->page();

    connect(webView, &QWebEngineView::titleChanged, this, [this, webView](const QString &title)
    {
        int index = ui->tabWidget->indexOf(webView);
        if (index != -1)
        {
            ui->tabWidget->tabBar()->setTabText(index, title);
            ui->tabWidget->tabBar()->setTabToolTip(index, title);
        }
        if (ui->tabWidget->tabBar()->currentIndex() == index) { emit titleChanged(title); }
    });
    connect(webView, &QWebEngineView::urlChanged, this, [this, webView](const QUrl &url)
    {
        int index = ui->tabWidget->indexOf(webView);
        if (index != -1) { ui->tabWidget->tabBar()->setTabData(index, url); }
        if (ui->tabWidget->tabBar()->currentIndex() == index) { emit urlChanged(url); }
    });
    connect(webView, &QWebEngineView::loadProgress, this, [this, webView](int progress)
    {
        if (ui->tabWidget->tabBar()->currentIndex() == ui->tabWidget->indexOf(webView)) { emit loadProgress(progress); }
    });
    connect(webPage, &QWebEnginePage::linkHovered, this, [this, webView](const QString &url)
    {
        if (ui->tabWidget->tabBar()->currentIndex() == ui->tabWidget->indexOf(webView)) { emit linkHovered(url); }
    });
    connect(webView, &WebView::favIconChanged, this, [this, webView](const QIcon &icon)
    {
        int index = ui->tabWidget->indexOf(webView);
        if (index != -1) { ui->tabWidget->tabBar()->setTabIcon(index, icon); }
        if (ui->tabWidget->tabBar()->currentIndex() == index) { emit favIconChanged(icon); }
    });
    connect(webView, &WebView::webActionEnabledChanged, this, [this, webView](QWebEnginePage::WebAction action, bool enabled)
    {
        if (ui->tabWidget->tabBar()->currentIndex() ==  ui->tabWidget->indexOf(webView)) { emit webActionEnabledChanged(action,enabled); }
    });
    connect(webPage, &QWebEnginePage::windowCloseRequested, this, [this, webView]()
    {
        int index = ui->tabWidget->indexOf(webView);
        if (index >= 0) { closeTab(index); }
    });
    connect(webView, &WebView::devToolsRequested, this, &MainWindow::devToolsRequested);
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    connect(webPage, &QWebEnginePage::findTextFinished, this, [this, webView](const QWebEngineFindTextResult &result)
    {
        if (ui->tabWidget->tabBar()->currentIndex() == ui->tabWidget->indexOf(webView)) { emit findTextFinished(result); }
    });
    #endif
}
/*****************************************************************************/
/**
 * @brief MainWindow::getDownloadTab
 * @return
 */
int MainWindow::getDownloadTab()
{
    return TabDownload;
}
/*****************************************************************************/
/**
 * @brief MainWindow::createTab
 * @return
 */
WebView *MainWindow::createTab()
{
    WebView *webView = createBackgroundTab();
    ui->tabWidget->setCurrentWidget(webView);
    return webView;
}
/*****************************************************************************/
/**
 * @brief MainWindow::createBackgroundTab
 * @return
 */
WebView *MainWindow::createBackgroundTab()
{
    if (ui->actionIncognito->isChecked())
    {
        if (!myOtrProfile) { myOtrProfile.reset(new QWebEngineProfile); }
        QObject::connect(myOtrProfile.get(), &QWebEngineProfile::downloadRequested, myDownloadManagerWidget.data(), &DownloadManagerWidget::downloadRequested);
    }
    else
    {
        myProfile = QWebEngineProfile::defaultProfile();
        QObject::connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested, myDownloadManagerWidget.data(), &DownloadManagerWidget::downloadRequested);
    }
    WebView *webView = new WebView;
    WebPage *webPage = new WebPage(myProfile, webView);
    webView->setPage(webPage);
    setupView(webView);
    int index = ui->tabWidget->addTab(webView, tr("(Untitled)"));
    // Set Color
    /*
Qt::white       3   White (#ffffff)
Qt::black       2   Black (#000000)
Qt::red         7   Red (#ff0000)
Qt::darkRed     13  Dark red (#800000)
Qt::green       8   Green (#00ff00) Lime
Qt::darkGreen   14  Dark green (#008000)
Qt::blue        9   Blue (#0000ff)
Qt::darkBlue    15  Dark blue (#000080)
Qt::cyan        10  Cyan (#00ffff)
Qt::darkCyan    16  Dark cyan (#008080)
Qt::magenta     11  Magenta (#ff00ff)
Qt::darkMagenta 17  Dark magenta (#800080)
Qt::yellow      12    Yellow (#ffff00)
Qt::darkYellow  18  Dark yellow (#808000)
Qt::gray        5   Gray (#a0a0a4)
Qt::darkGray    4   Dark gray (#808080)
Qt::lightGray   6   Light gray (#c0c0c0)
Qt::transparent 19a transparent black value (i.e., QColor(0, 0, 0, 0))
Qt::color000 pixel value (for bitmaps)
Qt::color111 pixel value (for bitmaps)
     */
    if (ui->actionIncognito->isChecked()) { ui->tabWidget->tabBar()->setTabTextColor(index, Qt::darkRed); }
    else                                  { ui->tabWidget->tabBar()->setTabTextColor(index, Qt::darkGreen); }
    // Set Icon
    ui->tabWidget->tabBar()->setTabIcon(index, webView->favIcon());
    // Workaround for QTBUG-61770
    webView->resize(ui->tabWidget->currentWidget()->size());
    if (!ui->lineEditDefaultUrl->text().isEmpty()) { webView->setUrl(ui->lineEditDefaultUrl->text()); }
    webView->show();
    return webView;
}
/*****************************************************************************/
/**
 * @brief MainWindow::reloadAllTabs
 */
void MainWindow::reloadAllTabs()
{
    for (int i = 0; i < ui->tabWidget->tabBar()->count(); ++i) { webView(i)->reload(); }
}
/*****************************************************************************/
/**
 * @brief MainWindow::closeOtherTabs
 * @param index
 */
void MainWindow::closeOtherTabs(int index)
{
    for (int i = ui->tabWidget->tabBar()->count() - 1; i > index; --i) { closeTab(i); }
    for (int i = index - 1; i >= 0; --i) { closeTab(i); }
}
/*****************************************************************************/
/**
 * @brief MainWindow::closeTab
 * @param index
 */
void MainWindow::closeTab(int index)
{
    if (TabBookmarks == index)
    {
        ui->tabWidget->tabBar()->setTabVisible(index, false);
        return;
    }
    if (TabDownload == index)
    {
        ui->tabWidget->tabBar()->setTabVisible(index, false);
        return;
    }
    if (TabHistory == index)
    {
        ui->tabWidget->tabBar()->setTabVisible(index, false);
        return;
    }
    if (TabSettings == index)
    {
        ui->tabWidget->tabBar()->setTabVisible(index, false);
        return;
    }
    if (TabWizard == index)
    {
        ui->tabWidget->tabBar()->setTabVisible(index, false);
        return;
    }
    if (WebView *view = webView(index))
    {
        bool hasFocus = view->hasFocus();
        ui->tabWidget->tabBar()->removeTab(index);
        if (hasFocus && ui->tabWidget->tabBar()->count() > 0) { currentWebView()->setFocus(); }
        if (ui->tabWidget->tabBar()->count() == 0) { createTab(); }
        view->deleteLater();
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::cloneTab
 * @param index
 */
void MainWindow::cloneTab(int index)
{
    if (WebView *view = webView(index))
    {
        WebView *tab = createTab();
        tab->setUrl(view->url());
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::setUrl
 * @param url
 */
void MainWindow::setUrl(const QUrl &url)
{
    if (WebView *view = currentWebView())
    {
        view->setUrl(url);
        view->setFocus();
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::triggerWebPageAction
 * @param action
 */
void MainWindow::triggerWebPageAction(QWebEnginePage::WebAction action)
{
    if (WebView *webView = currentWebView())
    {
        webView->triggerPageAction(action);
        webView->setFocus();
    }
}
/*****************************************************************************/
/**
 * @brief MainWindow::nextTab
 */
void MainWindow::nextTab()
{
    int next = ui->tabWidget->tabBar()->currentIndex() + 1;
    if (next == ui->tabWidget->tabBar()->count()) { next = 0; }
    ui->tabWidget->tabBar()->setCurrentIndex(next);
}
/*****************************************************************************/
/**
 * @brief MainWindow::previousTab
 */
void MainWindow::previousTab()
{
    int next = ui->tabWidget->tabBar()->currentIndex() - 1;
    if (next < 0) { next = ui->tabWidget->tabBar()->count() - 1; }
    ui->tabWidget->tabBar()->setCurrentIndex(next);
}
/*****************************************************************************/
/**
 * @brief MainWindow::reloadTab
 * @param index
 */
void MainWindow::reloadTab(int index)
{
    if (WebView *view = webView(index)) { view->reload(); }
}
#if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
/*****************************************************************************/
/**
 * @brief MainWindow::onCustomContextMenuRequested
 * @param pos
 */
void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    const QTreeWidgetItem *item = myTreeWidget->itemAt(pos);
    if (!item) { return; }
    const QString url = item->text(1);
    QMenu contextMenu;
    QAction *copyAction = contextMenu.addAction(QObject::tr("Copy Link to Clipboard"));
    QAction *openAction = contextMenu.addAction(QObject::tr("Open"));
    //QAction *addAction = contextMenu.addAction(QObject::tr("Add"));
    QAction *action = contextMenu.exec(myTreeWidget->viewport()->mapToGlobal(pos));
    // clipboard FIXME openURL in new tab
         if (action == copyAction) { QGuiApplication::clipboard()->setText(url); }
    else if (action == openAction) { QDesktopServices::openUrl(QUrl(url)); }
    //else if (action == addAction)  { myTreeWidget->addBookmark(); }
}
#endif // !QT_NO_CONTEXTMENU && !QT_NO_CLIPBOARD
/*****************************************************************************/
/**
 * @brief MainWindow::onDelete
 */
void MainWindow::onDelete()
{
    myTreeWidget->deleteBookmark();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onClear
 */
void MainWindow::onClear()
{
    myTreeWidget->setTitle("");
    myTreeWidget->setUrl("");
    ui->lineEditBookmarksTitle->setText("");
    ui->lineEditBookmarksUrl->setText("");
}
/*****************************************************************************/
/**
 * @brief MainWindow::onSave
 */
void MainWindow::onSave()
{
    myTreeWidget->save();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onTreeViewClick
 */
void MainWindow::onTreeViewChanged()
{
    setTitle(myTreeWidget->currentItem()->text(0));
    setLineEditUrl(myTreeWidget->currentItem()->text(1));
}
/*****************************************************************************/
/**
 * @brief MainWindow::onAddSeparator
 */
void MainWindow::onAddSeparator()
{
    myTreeWidget->setTitle(ui->lineEditBookmarksTitle->text());
    myTreeWidget->setUrl(ui->lineEditBookmarksUrl->text());
    myTreeWidget->addSeparator();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onAddBookmark
 */
void MainWindow::onAddBookmark()
{
    myTreeWidget->setTitle(ui->lineEditBookmarksTitle->text());
    myTreeWidget->setUrl(ui->lineEditBookmarksUrl->text());
    myTreeWidget->addBookmark();
}
/*****************************************************************************/
/**
 * @brief MainWindow::onAddFolder
 */
void MainWindow::onAddFolder()
{
    myTreeWidget->setTitle(ui->lineEditBookmarksTitle->text());
    myTreeWidget->setUrl(ui->lineEditBookmarksUrl->text());
    myTreeWidget->addFolder();
}
/*****************************************************************************/
/**
 * @brief MainWindow::getMainWindow
 * @return
 */
QMainWindow *MainWindow::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
    {
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w)) { return mainWin; }
    }
    return nullptr;
}
/*****************************************************************************/
/**
 * @brief MainWindow::setTitle
 * @param thisTile
 */
void MainWindow::setTitle(const QString &thisTile)
{
    ui->lineEditBookmarksTitle->setText(thisTile);
}
/*****************************************************************************/
/**
 * @brief MainWindow::setLineEditUrl
 * @param thisUrl
 */
void MainWindow::setLineEditUrl(const QString &thisUrl)
{
    ui->lineEditBookmarksUrl->setText(thisUrl);
}
/*****************************************************************************/
/**
 * @brief MainWindow::on_spinBoxFontPointSize_valueChanged
 * @param thisFontPointSize
 */
void MainWindow::onSpinBoxFontPointSizeValueChanged(int thisFontPointSize)
{
    setTabFontPointSize(thisFontPointSize);
    changeFonts();
}
/*****************************************************************************/
/**
 * @brief MainWindow::on_fontComboBox_currentFontChanged
 * @param f
 */
void MainWindow::onFontComboBoxCurrentFontChanged(const QFont &f)
{
    setTabFontName(f);
    changeFonts();
}
/*****************************************************************************/
/**
 * @brief MainWindow::setTabFontPointSize
 * @param thisFontPointSize
 */
void MainWindow::setTabFontPointSize(int thisFontPointSize)
{
    myTabFont->setPointSize(thisFontPointSize);
}
/*****************************************************************************/
/**
 * @brief MainWindow::setTabFontName
 * @param f
 */
void MainWindow::setTabFontName(const QFont &f)
{
    myTabFont = new QFont(f);
    myTabFontName = f.family();
}
/*****************************************************************************/
/**
 * @brief MainWindow::setFontByName
 * @param thisFontName
 */
void MainWindow::setFontByName(const QString &thisFontName)
{
    myTabFont = new QFont(thisFontName);
    myTabFontName = thisFontName;
}
/*****************************************************************************/
/**
 * @brief MainWindow::changeFonts
 */
void MainWindow::changeFonts()
{
    //ui->tabWidget->setFont(QFont(*myTabFont));
    // FIXME add all here for same size
}
/******************************* End of File *********************************/
