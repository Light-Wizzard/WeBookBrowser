#include "BookmarkTreeWidgetView.h"

/*****************************************************************************/
/**
 * @brief TreeWidgetView::TreeWidgetView
 */
BookmarkTreeWidgetView::BookmarkTreeWidgetView(QStatusBar *statusbar) : myStatusbar(statusbar)
{
    setColumnCount(2);
    QStringList labels;
    labels << QObject::tr("Title") << QObject::tr("Location");
    header()->setSectionResizeMode(QHeaderView::Stretch);
    setHeaderLabels(labels);
    #if !defined(QT_NO_CONTEXTMENU) && !defined(QT_NO_CLIPBOARD)
    setContextMenuPolicy(Qt::CustomContextMenu);
    // FIXME Context Menu
    //QObject::connect(myBookmarkView, &QWidget::customContextMenuRequested, this, &Browser::onCustomContextMenuRequested);
    #endif
    QStyle *style = this->style();
    myBookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
    myFolderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    myFolderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),   QIcon::Normal, QIcon::On);
    //
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    //
    setBookmarkName("bookmarks.xbel");
    searchBookmarkFile();
    if (myBookmarkFile.isEmpty())
    {
        // if it cannot find one then use the built-in one
        myBookmarkFile = ":/bookmarks.xbel";
    }
    QFile file(myBookmarkFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(myBookmarkFile), file.errorString()));
        return;
    }
    myXmlReader = new XbelTreeReader(this);

    if (!myXmlReader->read(&file))
    {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"), tr("Parse error in file %1:\n\n%2").arg(QDir::toNativeSeparators(myBookmarkFile), myXmlReader->errorString()));
    }
    else
    {
        myStatusbar->showMessage(tr("File loaded"), 2000);
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::dragEnterEvent
 * @param event
 */
void BookmarkTreeWidgetView::dragEnterEvent(QDragEnterEvent *event)
{
    draggedItem = currentItem();
    QTreeWidget::dragEnterEvent(event);
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::dropEvent
 * @param event
 */
void BookmarkTreeWidgetView::dropEvent(QDropEvent *event)
{
    QModelIndex droppedIndex = indexAt(event->position().toPoint());

    if( !droppedIndex.isValid() ) { return; }

    if(draggedItem)
    {
        QTreeWidgetItem* dParent = draggedItem->parent();
        if(dParent)
        {
            if(itemFromIndex(droppedIndex.parent()) != dParent) { return; }
            dParent->removeChild(draggedItem);
            dParent->insertChild(droppedIndex.row(), draggedItem);
        }
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::favIcon
 * @return
 */
QIcon BookmarkTreeWidgetView::favIcon() const
{
    // FIXME icon
    static QIcon favIcon(QStringLiteral(":addressbook.png"));
    return favIcon;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::searchBookmarkFile
 * @return
 */
QString BookmarkTreeWidgetView::searchBookmarkFile()
{
    //QString theAppDataLocation = getAppDataLocation();
    // Relative to Bin Dir
    QString theBinDir = QCoreApplication::applicationDirPath();
    QString theFileName = getBookmarkName();
    QStringList theSearchList;
    theSearchList.append(theBinDir);
    theSearchList.append(theBinDir + "/" + getBookmarkFolder());
    theSearchList.append(theBinDir + "/../" + getBookmarkFolder());
    theSearchList.append(theBinDir + QString("%1%2%3%4").arg(QDir::separator(), QCoreApplication::applicationName(), QDir::separator(), getBookmarkFolder())); // for development with shadow build (Linux)
    theSearchList.append(theBinDir + QString("/../%1/%2").arg(QCoreApplication::applicationName(), getBookmarkFolder())); // for development with shadow build (Linux)
    theSearchList.append(theBinDir + QString("/../../%1/%2").arg(QCoreApplication::applicationName(), getBookmarkFolder())); // for development with shadow build (Windows)
    theSearchList.append(QDir::rootPath() + getBookmarkFolder() + "/opt");
    theSearchList.append(QDir::rootPath() + getBookmarkFolder());
    //
    foreach (QString thisDir, theSearchList)
    {
        QFile theBookmarkFile(thisDir + QDir::separator() + theFileName);
        if (theBookmarkFile.exists())
        {
            theFileName = QDir(theBookmarkFile.fileName()).canonicalPath();
            QFileInfo myFileInfo(theFileName);
            theFileName = myFileInfo.absoluteFilePath();
            setBookmarkFile(theFileName);
            setBookmarkPath(myFileInfo.absolutePath());
            qDebug() << QString("%1: %2").arg(QObject::tr("Using Bookmark file"), qPrintable(theFileName));
            return theFileName;
        }
    }
    // not found, check main theAppName and folder should be the same
    foreach (QString thisDir, theSearchList)
    {
        qWarning() << QString("%1: %2/%3").arg(QObject::tr("not found"), thisDir, theFileName);
    }
    qFatal("%s", qPrintable(QString("%1: %2").arg(QObject::tr("Cannot find Bookmark file"), theFileName)));
    return nullptr;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setBookmarkFile
 * @param thisBookmarkFile
 */
void BookmarkTreeWidgetView::setBookmarkFile(const QString &thisBookmarkFile)
{
    if (myBookmarkFile != thisBookmarkFile)
    {
        myBookmarkFile = thisBookmarkFile;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getBookmarkFile
 * @return
 */
QString BookmarkTreeWidgetView::getBookmarkFile()
{
    return myBookmarkFile;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setBookmarkPath
 * @param thisBookmarkPath
 */
void BookmarkTreeWidgetView::setBookmarkPath(const QString &thisBookmarkPath)
{
    if (myBookmarkPath != thisBookmarkPath)
    {
        myBookmarkPath = thisBookmarkPath;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getBookmarkPath
 * @return
 */
QString BookmarkTreeWidgetView::getBookmarkPath()
{
    return myBookmarkPath;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setBookmarkFolder
 * @param thisBookmarkFolder
 */
void BookmarkTreeWidgetView::setBookmarkFolder(const QString &thisBookmarkFolder)
{
    if (myBookmarkFolder != thisBookmarkFolder)
    {
        myBookmarkFolder = thisBookmarkFolder;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getBookmarkFolder
 * @return
 */
QString BookmarkTreeWidgetView::getBookmarkFolder()
{
    return myBookmarkFolder;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setBookmarkName
 * @param thisBookmarkName
 */
void BookmarkTreeWidgetView::setBookmarkName(const QString &thisBookmarkName)
{
    if (myBookmarkName != thisBookmarkName)
    {
        myBookmarkName = thisBookmarkName;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getBookmarkFileName
 * @return
 */
QString BookmarkTreeWidgetView::getBookmarkName()
{
    return myBookmarkName;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setTitle
 * @param thisTitle
 */
void BookmarkTreeWidgetView::setTitle(const QString &thisTitle)
{
    if (myTitle != thisTitle)
    {
        myTitle = thisTitle;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getTitle
 * @return
 */
QString BookmarkTreeWidgetView::getTitle()
{
    return myTitle;
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::setUrl
 * @param thisUrl
 */
void BookmarkTreeWidgetView::setUrl(const QString &thisUrl)
{
    if (myUrl != thisUrl)
    {
        myUrl = thisUrl;
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::getUrl
 * @return
 */
QString BookmarkTreeWidgetView::getUrl()
{
    return myUrl;
}
/*****************************************************************************/
/**
 * @brief BookmarkTreeWidgetView::addTreeRoot
 * @param thisTitle
 * @param thisUrl
 */
void BookmarkTreeWidgetView::addTreeRoot(QString thisTitle, QString thisUrl)
{
    // QTreeWidgetItem(QTreeWidget * parent, int type = Type)
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(this);

    // QTreeWidgetItem::setText(int column, const QString & text)
    treeItem->setText(0, thisTitle);
    treeItem->setText(1, thisUrl);
    addTreeChild("bookmark", treeItem, thisTitle + "A", "Child_first");
    //addTreeChild(treeItem, theTitle + "B", "Child_second");
}
/*****************************************************************************/
/**
 * @brief BookmarkTreeWidgetView::addTreeChild
 * @param thisBookmark
 * @param thisParent
 * @param thisTitle
 * @param thisUrl
 */
void BookmarkTreeWidgetView::addTreeChild(QString thisBookmark, QTreeWidgetItem *thisParent, QString thisTitle, QString thisUrl)
{
    QTreeWidgetItem *theBookmark = createChildItem(thisParent, thisBookmark);
    if (thisBookmark == "bookmark")
    {
        theBookmark->setText(1, thisUrl);
        theBookmark->setText(0, thisTitle);
        theBookmark->setIcon(0, myBookmarkIcon);
        theBookmark->setFlags(theBookmark->flags() | Qt::ItemIsEditable);
    }
    else if (thisBookmark == "folder")
    {
        theBookmark->setExpanded(true);
        theBookmark->setIcon(0, myFolderIcon);
        theBookmark->setFlags(theBookmark->flags() | Qt::ItemIsEditable);
        theBookmark->setText(0, thisTitle);
    }
    else if (thisBookmark == "separator")
    {
        theBookmark->setFlags(theBookmark->flags() & ~Qt::ItemIsSelectable);
        theBookmark->setText(0, QString(30, u'\xB7'));
    }
}
/*****************************************************************************/
/**
 * @brief BookmarkTreeWidgetView::createChildItem
 * @param thisItem
 * @param thisRoleName
 * @return
 */
QTreeWidgetItem *BookmarkTreeWidgetView::createChildItem(QTreeWidgetItem *thisItem, QString thisRoleName)
{
    QTreeWidgetItem *childItem;
    if (thisItem)
    {
        childItem = new QTreeWidgetItem(thisItem);
    }
    else
    {
        childItem = new QTreeWidgetItem(this);
    }
    childItem->setData(0, Qt::UserRole, thisRoleName);
    return childItem;
}
/*****************************************************************************/
/**
 * @brief BookmarkTreeWidgetView::addSeparator
 */
void BookmarkTreeWidgetView::addSeparator()
{
    qDebug() << "Separator";

    if (currentItem())
    {
        addTreeChild("separator", currentItem(), myTitle, myUrl);
    }
    else
    {
        //addTreeChild(itemAt(0,0), myTitle, myUrl);
        addTreeChild("separator", topLevelItem(0), myTitle, myUrl);
    }
    save();
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::addBookmark
 * @param thisUrl
 */
void BookmarkTreeWidgetView::addBookmark()
{
    qDebug() << "Bookmark";
    if (myTitle.isEmpty()) { return; }
    if (myUrl.isEmpty()) { return; }

    if (currentItem())
    {
        addTreeChild("bookmark", currentItem(), myTitle, myUrl);
    }
    else
    {
        //addTreeChild(itemAt(0,0), myTitle, myUrl);
        addTreeChild("bookmark", topLevelItem(0), myTitle, myUrl);
    }
    save();
}
/*****************************************************************************/
/**
 * @brief BookmarkTreeWidgetView::addFolder
 */
void BookmarkTreeWidgetView::addFolder()
{
    qDebug() << "Folder";
    if (myTitle.isEmpty()) { return; }
    if (currentItem())
    {
        addTreeChild("folder", currentItem(), myTitle, myUrl);
    }
    else
    {
        //addTreeChild(itemAt(0,0), myTitle, myUrl);
        addTreeChild("folder", topLevelItem(0), myTitle, myUrl);
    }
    save();
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::save
 */
void BookmarkTreeWidgetView::save()
{
    QFile file(myBookmarkFile);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("QXmlStream Bookmarks"), tr("Cannot write file %1:\n%2.").arg(QDir::toNativeSeparators(myBookmarkFile), file.errorString()));
        return;
    }
    XbelTreeWriter writer(this);
    if (writer.writeFile(&file))
    {
        myStatusbar->showMessage(tr("File saved"), 6000);
    }
    else
    {
        myStatusbar->showMessage(tr("Failed to save File"), 9000);
    }
}
/*****************************************************************************/
/**
 * @brief TreeWidgetView::deleteBookmark
 */
void BookmarkTreeWidgetView::deleteBookmark()
{
    if (myTitle.isEmpty()) { return; }
    if (myUrl.isEmpty()) { return; }

    if (currentItem())
    {
       removeItemWidget(currentItem(), currentColumn());
    }
    save();
}
/******************************* End of File *********************************/
