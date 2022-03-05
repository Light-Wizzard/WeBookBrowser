#ifndef BOOKMARKTREEWIDGETVIEW_H
#define BOOKMARKTREEWIDGETVIEW_H

#include <QCoreApplication>
#include <QGuiApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDropEvent>
#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QHeaderView>
#include <QDesktopServices>
#include <QMenu>
#include <QMainWindow>

#include "XbelTreeReader.h"
#include "XbelTreeWriter.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE
/*****************************************************************************/
/**
 * @brief The TreeWidgetView class
 */
class BookmarkTreeWidgetView : public QTreeWidget
{
        Q_OBJECT
    public:
        BookmarkTreeWidgetView(QStatusBar *statusbar);
        // Icon for Tab
        QIcon favIcon() const;
        // search for Bookmark File
        QString searchBookmarkFile();
        // Bookmark Name
        void setBookmarkName(const QString &thisBookmarkName);
        QString getBookmarkName();
        // Bookmark Folder
        void setBookmarkFolder(const QString &thisBookmarkFolder);
        QString getBookmarkFolder();
        // Bookmark File
        void setBookmarkFile(const QString &thisBookmarkFile);
        QString getBookmarkFile();
        // Bookmark Path
        void setBookmarkPath(const QString &thisBookmarkPath);
        QString getBookmarkPath();
        // Add
        void addTreeChild(QString thisBookmark, QTreeWidgetItem *thisParent, QString thisTitle, QString thisUrl);
        void addTreeRoot(QString thisTitle, QString thisUrl);
        // Title
        void setTitle(const QString &thisTitle);
        QString getTitle();
        // Url
        void setUrl(const QString &thisUrl);
        QString getUrl();
        //
        void addBookmark();
        void save();
        //
        void deleteBookmark();
        //
        QTreeWidgetItem *createChildItem(QTreeWidgetItem *thisItem, QString thisRoleName);

        void addFolder();
        void addSeparator();

    protected:
        virtual void dragEnterEvent(QDragEnterEvent *event) override;
        virtual void dropEvent(QDropEvent *event) override;

    private:
        QStatusBar      *myStatusbar;
        QTreeWidgetItem *draggedItem;
        QString          myBookmarkName   = ""; //!< \c myBookmarkName   @brief Bookmark Name
        QString          myBookmarkFile   = ""; //!< \c myBookmarkFile   @brief Bookmark File
        QString          myBookmarkPath   = ""; //!< \c myBookmarkPath   @brief Bookmark Path
        QString          myBookmarkFolder = ""; //!< \c myBookmarkFolder @brief Bookmark Folder
        QString          myTitle          = ""; //!< \c myTitle          @brief Title
        QString          myUrl            = ""; //!< \c myUrl            @brief Url
        XbelTreeReader  *myXmlReader;           //!< \c myXmlReader      @brief Xml Reader
        QIcon            myBookmarkIcon;        //!< \c myBookmarkIcon   @brief Bookmark Icon
        QIcon            myFolderIcon;          //!< \c myFolderIcon     @brief Folder Icon

}; // end class TreeWidgetView
#endif // BOOKMARKTREEWIDGETVIEW_H
/******************************* End of File *********************************/
