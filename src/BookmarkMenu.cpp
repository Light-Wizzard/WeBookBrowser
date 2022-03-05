#include "BookmarkMenu.h"

/*****************************************************************************/
/**
 * @brief BookmarkMenuWidgetView::BookmarkMenuWidgetView
 * @param thisMenuWidget
 * @param parent
 */
BookmarkMenu::BookmarkMenu(QMenu *thisMenuWidget, QObject *parent) : QObject(parent), myMenuWidget(thisMenuWidget)
{
    readMenu();
}
/*****************************************************************************/
/**
 * @brief BookmarkMenuWidgetView::readMenu
 */
void BookmarkMenu::readMenu()
{
    setBookmarkName("bookmarks.xbel");
    searchBookmarkFile();
    if (myBookmarkFile.isEmpty())
    {
        // if it cannot find one then use the built-in one FIXME create a blank
        myBookmarkFile = ":/bookmarks.xbel";
    }
    QFile file(myBookmarkFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        //QMessageBox::warning(this, tr("QXmlStream Bookmarks"), tr("Cannot read file %1:\n%2.").arg(QDir::toNativeSeparators(myBookmarkFile), file.errorString()));
        return;
    }
    xmlReader = new XbelMenuReader(myMenuWidget);

    if (!xmlReader->readXmlFile(&file))
    {
        //QMessageBox::warning(this, tr("QXmlStream Bookmarks"), tr("Parse error in file %1:\n\n%2").arg(QDir::toNativeSeparators(myBookmarkFile), xmlReader->errorString()));
    }
    else
    {
        //statusBar()->showMessage(tr("File loaded"), 2000);
    }
    // FIXME disconnect if this is not first time?
    QObject::connect(xmlReader, &XbelMenuReader::handleOpenBookmark, this, &BookmarkMenu::onOpenBookmark);
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::openBookmark
 * @param thisUrl
 */
void BookmarkMenu::onOpenBookmark(const QString &thisUrl)
{
    qDebug() << "onOpenBookmark(" << thisUrl << ")";
    emit handleOpenBookmark(thisUrl);
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::favIcon
 * @return
 */
QIcon BookmarkMenu::favIcon() const
{
    // FIXME icon
    static QIcon favIcon(QStringLiteral(":addressbook.png"));
    return favIcon;
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::searchBookmarkFile
 * @return
 */
QString BookmarkMenu::searchBookmarkFile()
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
 * @brief MenutWidgetView::setBookmarkFile
 * @param thisBookmarkFile
 */
void BookmarkMenu::setBookmarkFile(const QString &thisBookmarkFile)
{
    if (myBookmarkFile != thisBookmarkFile)
    {
        myBookmarkFile = thisBookmarkFile;
    }
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::getBookmarkFile
 * @return
 */
QString BookmarkMenu::getBookmarkFile()
{
    return myBookmarkFile;
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::setBookmarkPath
 * @param thisBookmarkPath
 */
void BookmarkMenu::setBookmarkPath(const QString &thisBookmarkPath)
{
    if (myBookmarkPath != thisBookmarkPath)
    {
        myBookmarkPath = thisBookmarkPath;
    }
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::getBookmarkPath
 * @return
 */
QString BookmarkMenu::getBookmarkPath()
{
    return myBookmarkPath;
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::setBookmarkFolder
 * @param thisBookmarkFolder
 */
void BookmarkMenu::setBookmarkFolder(const QString &thisBookmarkFolder)
{
    if (myBookmarkFolder != thisBookmarkFolder)
    {
        myBookmarkFolder = thisBookmarkFolder;
    }
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::getBookmarkFolder
 * @return
 */
QString BookmarkMenu::getBookmarkFolder()
{
    return myBookmarkFolder;
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::setBookmarkName
 * @param thisBookmarkName
 */
void BookmarkMenu::setBookmarkName(const QString &thisBookmarkName)
{
    if (myBookmarkName != thisBookmarkName)
    {
        myBookmarkName = thisBookmarkName;
    }
}
/*****************************************************************************/
/**
 * @brief MenutWidgetView::getBookmarkFileName
 * @return
 */
QString BookmarkMenu::getBookmarkName()
{
    return myBookmarkName;
}
/******************************* End of File *********************************/
