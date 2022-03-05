#ifndef BOOKMARKMENU_H
#define BOOKMARKMENU_H

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

#include "XbelMenuReader.h"
#include "XbelTreeWriter.h"

/*****************************************************************************/
/**
 * @brief The MenutWidgetView class
 */
class BookmarkMenu : public QObject
{
        Q_OBJECT
    public:
        explicit BookmarkMenu(QMenu *thisMenuWidget, QObject *parent = nullptr);

        QIcon favIcon() const;
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
        // Read Menu
        void readMenu();

    public slots:
        void onOpenBookmark(const QString &thisUrl);

    signals:
        void handleOpenBookmark(const QString &thisUrl);

    private:
        QString          myBookmarkName   = ""; //!< \c myBookmarkName   @brief Bookmark Name
        QString          myBookmarkFile   = ""; //!< \c myBookmarkFile   @brief Bookmark File
        QString          myBookmarkPath   = ""; //!< \c myBookmarkPath   @brief Bookmark Path
        QString          myBookmarkFolder = ""; //!< \c myBookmarkFolder @brief Bookmark Folder
        QString          myTitle          = "";
        QString          myUrl            = "";
        XbelMenuReader  *xmlReader;
        QMenu           *myMenuWidget;

}; // end class MenutWidgetView
#endif // BOOKMARKMENU_H
/******************************* End of File *********************************/
