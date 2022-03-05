#ifndef BOOKMARKVIEW_H
#define BOOKMARKVIEW_H

#include <QMainWindow>
#include <QObject>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include "BookmarkTreeWidgetView.h"
#include "MainWindow.h"
QT_BEGIN_NAMESPACE
namespace Ui { class BookmarkView; }
QT_END_NAMESPACE
/*****************************************************************************/
/**
 * @brief The BookmarkView class
 */
class BookmarkView : public QWidget
{
        Q_OBJECT
    public:
        explicit BookmarkView(QWidget *parent = nullptr);

        QIcon favIcon() const;

        QMainWindow *getMainWindow();

        void setTitle(const QString &thisTile);
        void setUrl(const QString &thisUrl);

    public slots:
        void onAddBookmark();
        void onSave();
        void onDelete();
        void onTreeViewChanged();
        #if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
        void onCustomContextMenuRequested(const QPoint &pos);
        #endif

    private:
        Ui::BookmarkView *ui;

}; // end class BookmarkView
#endif // BOOKMARKVIEW_H
/******************************* End of File *********************************/
