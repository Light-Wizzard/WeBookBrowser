#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QObject>
#include <QCloseEvent>
#include <QEvent>
//#include <QDesktopWidget>
#include <QDesktopServices>
#include <QWebEnginePage>
#include <QTime>
#include <QTimer>
#include <QMenuBar>
#include <QProgressBar>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QString>
#include <QClipboard>
#include <QtWidgets>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QFrame>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QUrl>
#include <QSpinBox>
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    #include <QWebEngineFindTextResult>
#endif
#include <QWebEngineProfile>
#include <QVector>
//
#include "WebPage.h"
#include "WebView.h"
#include "BookmarkView.h"
#include "HelpView.h"
#include "DownloadList.h"
#include "DownloadManagerWidget.h"
#include "MyLanguageModel.h"
#include "BookmarkMenu.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class WebView;
/*****************************************************************************/
/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        /*!
            \enum MyTabs
            @brief Tab Indexes
         */
        enum MyTabs
        {
            TabBookmarks = 0, //!< \c TabBookmarks @brief Tab Bookmarks.
            TabDownload  = 1, //!< \c TabDownload  @brief Tab Download.
            TabHistory   = 2, //!< \c TabHistory   @brief Tab History.
            TabSettings  = 3, //!< \c TabSettings  @brief Tab Settings.
            TabWizard    = 4  //!< \c TabWizard    @brief Tab Wizard.
        }; // end enum MyTabs
        // Makes getting Tabs easier
        Q_ENUM(MyTabs)
        /*!
            \enum MyWizards
            @brief Wizards Indexes
         */
        enum MyWizards
        {
            WizardHelp  = 0, //!< \c WizardHelp  @brief Wizard Help.
            WizardAbout = 1  //!< \c WizardAbout @brief Tab Wizard About.
        }; // end enum MyTabs
        // Makes getting Wizards easier
        Q_ENUM(MyWizards)
        // Constants
        const QString MY_FONT_NAME_TABS = "Font";           //!< \c MY_FONT_NAME_TABS @brief Field Name for Font Name.
        const QString MY_FONT_SIZE_TABS = "Font-Size";      //!< \c MY_FONT_SIZE_TABS @brief Field Name for Font Size.
        const QString MY_LANGUAGE       = "Language";       //!< \c MY_LANGUAGE       @brief Field Name for Language.
        const QString MY_DEFAULT_URL    = "Default-URL";    //!< \c MY_DEFAULT_URL    @brief Field Name for Default URL.
        const QString MY_SEND_TO_URL    = "https://github.com/Light-Wizzard/WeBookClient/"; //!< \c MY_SEND_TO_URL @brief URL to send users to by Default.
        const QString MY_INCOGNITO      = "Incognito";      //!< \c MY_INCOGNITO @brief Field Name for Incognito.
        //
        QVector<MainWindow*> windows() { return myWindows; }
        //
        QSize sizeHint() const override;

        void onAddBookmark();
        QIcon favIcon() const;
        QMainWindow *getMainWindow();
        void setTitle(const QString &thisTile);
        void setLineEditUrl(const QString &thisUrl);

        // Precondition: The QWebEngineDownloadItem has been accepted.
        //DownloadWidget(QWebEngineDownloadItem *download, QWidget *parent = nullptr);
        //
        WebView *currentWebView() const;
        // Bookmark Tab
        int getBookmarkTab();
        BookmarkView *getBookmarkView();
        // Help Tab
        int getHelpTab();
        HelpView *getHelpView();
        // Download Tab
        int getDownloadTab();

        void addFeatureTabs();
        void closeAllTabs();
        QTabWidget *tabWidget() const;
        WebView   *currentTab() const;

        // Settings
        void readSettings();
        void writeSettings();
        // Tab Fonts
        void setTabFontPointSize(int thisFontPointSize);
        void setTabFontName(const QFont &f);
        void setFontByName(const QString &thisFontName);
        void changeFonts();
        //
        // download Manager
        QPointer<DownloadManagerWidget> downloadManagerWidget() { return myDownloadManagerWidget; }


    public slots:
        void onFind();
        void onDelete();
        void onSave();
        void onTreeViewChanged();
        void onClear();
        void onCloseTab();
        void onAbout();
        void onHelp();
        void onBackward();
        void onDownloads();
        void onFindNext();
        void onFindPrevious();
        void onForward();
        void onHideMenu();
        void onHideStatusbar();
        void onHideToolbar();
        void onHistoryViewer();
        void onIncognito();
        void onBookmarkManager();
        void onNewTab();
        void onNextTab();
        void onOpenFile();
        void onPreviousTab();
        void onQuit();
        void onReload();
        void onStop();
        void onZoomIn();
        void onZoomOut();
        void onZoomReset();
        void onSettings();
        void onAddFolder();
        void onAddSeparator();

    protected:
        void closeEvent(QCloseEvent *event) override;

    private slots:
        #if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
        void onCustomContextMenuRequested(const QPoint &pos);
        #endif
        void handleCurrentChanged(int index);
        void handleContextMenuRequested(const QPoint &pos);
        void cloneTab(int index);
        void closeOtherTabs(int index);
        void reloadAllTabs();
        void reloadTab(int index);
        void handleFileOpenTriggered();
        void handleFindActionTriggered();
        void handleWebViewLoadProgress(int);
        void handleWebViewTitleChanged(const QString &title);
        void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
        #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        void handleFindTextFinished(const QWebEngineFindTextResult &result);
        #endif
        // current tab/page slots
        void setUrl(const QUrl &url);
        void triggerWebPageAction(QWebEnginePage::WebAction action);

        WebView *createTab();
        WebView *createBackgroundTab();
        void closeTab(int index);
        void nextTab();
        void previousTab();
        //
        void onSpinBoxFontPointSizeValueChanged(int thisFontPointSize);
        void onFontComboBoxCurrentFontChanged(const QFont &f);
        void onCheckBoxIncognitoStateChanged(int thisState);

    signals:
        // current tab/page signals
        void linkHovered(const QString &thisLink);
        void loadProgress(int progress);
        void titleChanged(const QString &title);
        void urlChanged(const QUrl &url);
        void favIconChanged(const QIcon &icon);
        void webActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
        void devToolsRequested(QWebEnginePage *source);
        #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        void findTextFinished(const QWebEngineFindTextResult &result);
        #endif

    private:
        WebView *webView(int index) const;
        void setupView(WebView *webView);
        //
        Ui::MainWindow          *ui;
        QWebEngineProfile       *myProfile;
        QScopedPointer<QWebEngineProfile> myOtrProfile;
        QString                 myLastSearch;
        QLineEdit               *myLineEditUrl;
        QAction                 *myFavAction;
        //DownloadManagerWidget   *myDownloadManagerWidget;
        QPointer<DownloadManagerWidget>   myDownloadManagerWidget;

        QFont                   *myTabFont;
        QString                 myTabFontName;
        int                     myTabFontSize = 12;
        QVector<MainWindow*>    myWindows;
        MyLanguageModel         *myLanguageModel = nullptr;
        bool                    isDebugMessage = false;
        BookmarkMenu            *myBookmarkMenuWidgetView   = nullptr;
        BookmarkTreeWidgetView          *myTreeWidget               = nullptr;
        bool                    isIncognito    = false;
        int                     myWizardTabIndex = -1;
}; // end class MainWindow
#endif // MAINWINDOW_H
/******************************* End of File *********************************/
