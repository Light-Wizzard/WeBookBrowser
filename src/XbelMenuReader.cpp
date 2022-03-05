#include "XbelMenuReader.h"

/*****************************************************************************/
/**
 * @brief XbelMenuReader::XbelMenuReader
 * @param thisMenuWidget
 * @param parent
 */
XbelMenuReader::XbelMenuReader(QMenu *thisMenuWidget, QObject *parent) : QObject(parent), myMenuWidget(thisMenuWidget)
{

}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::~XbelMenuReader
 */
XbelMenuReader::~XbelMenuReader()
{

}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readXmlFile
 * @param device
 * @return
 */
bool XbelMenuReader::readXmlFile(QIODevice *device)
{
    myXml.setDevice(device);

    if (myXml.readNextStartElement())
    {
        if (myXml.name() == QLatin1String("xbel") && myXml.attributes().value(versionAttribute()) == QLatin1String("1.0"))
        {
            readXBEL();
        }
        else
        {
            myXml.raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
        }
    }

    return !myXml.error();
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::errorString
 * @return
 */
QString XbelMenuReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3").arg(myXml.errorString()).arg(myXml.lineNumber()).arg(myXml.columnNumber());
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readXBEL
 */
void XbelMenuReader::readXBEL()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("xbel"));
    // Set a bookmark menu under the Top level menu passed in
    myTopSubMenu = mySubMenu = myMenuWidget;
    // read XML
    while (myXml.readNextStartElement())
    {
             if (myXml.name() == QLatin1String("folder"))    { readFolder(); }
        else if (myXml.name() == QLatin1String("bookmark"))  { readBookmark(); }
        else if (myXml.name() == QLatin1String("separator")) { readSeparator(); }
        else                                                 { myXml.skipCurrentElement(); }
    }
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readFolder
 * @param item
 */
void XbelMenuReader::readFolder()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("folder"));
    isFolder = true;
    while (myXml.readNextStartElement())
    {
             if (myXml.name() == QLatin1String("title"))     { readTitle(); }
        else if (myXml.name() == QLatin1String("folder"))    { readFolder(); }
        else if (myXml.name() == QLatin1String("bookmark"))  { readBookmark(); }
        else if (myXml.name() == QLatin1String("separator")) { readSeparator(); }
        else                                                 { myXml.skipCurrentElement(); }
    }
    isFolderChanged = true;
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readTitle
 * @param item
 */
void XbelMenuReader::readTitle()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("title"));
    myTitle = myXml.readElementText();
    if (isFolder)
    {
        isFolder = false;
        isFolderTitle = true;
        if (myTitle.isEmpty())
        {
            mySubMenu = mySubMenu->addMenu(myTitle);
        }
        else
        {
            if (!isFolderChanged)
            {
                mySubMenu = mySubMenu->addMenu(myTitle);
            }
            else
            {
                isFolderChanged = false;
                mySubMenu = myTopSubMenu->addMenu(myTitle);
            }
        }
    }
    else
    {
        //if (isFolderTitle) { isFolderTitle = false; return; }
        if (isSeparator) {  isSeparator = false;  mySubMenu->addSeparator();  }

        auto bookmarkAction = new QAction(myTitle);
        //bookmarkAction->setIcon(QIcon(QStringLiteral(":go-bottom.png")));
        bookmarkAction->setToolTip(myUrl);
        mySubMenu->addAction(bookmarkAction);
        QString theUrl = myUrl;
        QObject::connect(bookmarkAction, &QAction::triggered, this, [this, theUrl]() { onOpenBookmark(theUrl); });
    }
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readBookmark
 * @param item
 */
void XbelMenuReader::readBookmark()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("bookmark"));
    isFolder = false; // this should be false when it gets here
    myUrl = myXml.attributes().value(hrefAttribute()).toString();

    while (myXml.readNextStartElement())
    {
        if (myXml.name() == QLatin1String("title")) { readTitle(); }
        else                                        { myXml.skipCurrentElement(); }
    }
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::readSeparator
 * @param item
 */
void XbelMenuReader::readSeparator()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("separator"));
    isFolder = false; isSeparator = true;
    // Called C++ object pointer is null warning
    myXml.skipCurrentElement();
}
/*****************************************************************************/
/**
 * @brief XbelMenuReader::openBookmark
 * @param thisUrl
 */
void XbelMenuReader::onOpenBookmark(const QString &thisUrl)
{
    qDebug() << "onOpenBookmark(" << thisUrl << ")";
    emit handleOpenBookmark(thisUrl);
}
/******************************* End of File *********************************/
