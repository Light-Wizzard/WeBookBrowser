#include "BookmarkView.h"
#include "ui_BookmarkView.h"
/*****************************************************************************/
/**
 * @brief BookmarkView::BookmarkView
 * @param parent
 */
BookmarkView::BookmarkView(QWidget *parent) : QWidget(parent), ui(new Ui::BookmarkView)
{
    ui->setupUi(this);
    // Signals
    connect(ui->treeWidget, &TreeWidgetView::currentItemChanged, this, &BookmarkView::onTreeViewChanged);
    connect(ui->pushButtonAdd,    &QPushButton::clicked, this, &BookmarkView::onAddBookmark);
    connect(ui->pushButtonSave,   &QPushButton::clicked, this, &BookmarkView::onSave);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &BookmarkView::onDelete);
    // Context Menu Right-Click
    #if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QWidget::customContextMenuRequested, this, &BookmarkView::onCustomContextMenuRequested);
    #endif

}
#if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
/*****************************************************************************/
/**
 * @brief BrowserWindow::onCustomContextMenuRequested
 * @param pos
 */
void BookmarkView::onCustomContextMenuRequested(const QPoint &pos)
{
    const QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);
    if (!item) { return; }
    const QString url = item->text(1);
    QMenu contextMenu;
    QAction *copyAction = contextMenu.addAction(QObject::tr("Copy Link to Clipboard"));
    QAction *openAction = contextMenu.addAction(QObject::tr("Open"));
    QAction *addAction = contextMenu.addAction(QObject::tr("Add"));
    QAction *action = contextMenu.exec(ui->treeWidget->viewport()->mapToGlobal(pos));
    // clipboard FIXME openURL in new tab
         if (action == copyAction) { QGuiApplication::clipboard()->setText(url); }
    else if (action == openAction) { QDesktopServices::openUrl(QUrl(url)); }
    else if (action == addAction)  { ui->treeWidget->addBookmark(); }
}
#endif // !QT_NO_CONTEXTMENU && !QT_NO_CLIPBOARD
/*****************************************************************************/
/**
 * @brief BookmarkView::onDelete
 */
void BookmarkView::onDelete()
{
    //myTreeWidget;
}
/*****************************************************************************/
/**
 * @brief BookmarkView::onSave
 */
void BookmarkView::onSave()
{
    ui->treeWidget->save();
}
/*****************************************************************************/
/**
 * @brief BookmarkView::onTreeViewClick
 */
void BookmarkView::onTreeViewChanged()
{
    setTitle(ui->treeWidget->currentItem()->text(0));
    setUrl(ui->treeWidget->currentItem()->text(1));
}
/*****************************************************************************/
/**
 * @brief BookmarkView::onAddBookmark
 */
void BookmarkView::onAddBookmark()
{
    ui->treeWidget->setTitle(ui->lineEditTitle->text());
    ui->treeWidget->setUrl(ui->lineEditUrl->text());
    ui->treeWidget->addBookmark();
}
/*****************************************************************************/
/**
 * @brief BookmarkView::favIcon
 * @return
 */
QIcon BookmarkView::favIcon() const
{
    // FIXME icon
    static QIcon favIcon(QStringLiteral(":addressbook.png"));
    return favIcon;
}
/*****************************************************************************/
/**
 * @brief BookmarkView::getMainWindow
 * @return
 */
QMainWindow *BookmarkView::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
    {
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w)) { return mainWin; }
    }
    return nullptr;
}
/*****************************************************************************/
/**
 * @brief BookmarkView::setTitle
 * @param thisTile
 */
void BookmarkView::setTitle(const QString &thisTile)
{
    ui->lineEditTitle->setText(thisTile);
}
/*****************************************************************************/
/**
 * @brief BookmarkView::setUrl
 * @param thisUrl
 */
void BookmarkView::setUrl(const QString &thisUrl)
{
    ui->lineEditUrl->setText(thisUrl);
}
/******************************* End of File *********************************/
