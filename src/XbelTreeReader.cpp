/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "XbelTreeReader.h"

/*****************************************************************************/
/**
 * @brief XbelTreeReader::XbelTreeReader
 * @param thisTreeWidget
 */
XbelTreeReader::XbelTreeReader(QTreeWidget *thisTreeWidget) : myTreeWidget(thisTreeWidget)
{
    QStyle *style = thisTreeWidget->style();

    myFolderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    myFolderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),   QIcon::Normal, QIcon::On);
    myBookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::read
 * @param thisDevice
 * @return
 */
bool XbelTreeReader::read(QIODevice *thisDevice)
{
    myXml.setDevice(thisDevice);
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
 * @brief XbelTreeReader::errorString
 * @return
 */
QString XbelTreeReader::errorString() const
{
    return QObject::tr("%1\n%2 %3, %4 %5").arg(myXml.errorString(), QObject::tr("Line"), QString::number(myXml.lineNumber()), QObject::tr("column"), QString::number(myXml.columnNumber()));
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::readXBEL
 */
void XbelTreeReader::readXBEL()
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("xbel"));

    while (myXml.readNextStartElement())
    {
             if (myXml.name() == QLatin1String("folder"))    { readFolder(0); }
        else if (myXml.name() == QLatin1String("bookmark"))  { readBookmark(0); }
        else if (myXml.name() == QLatin1String("separator")) { readSeparator(0); }
        else                                                 { myXml.skipCurrentElement(); }
    }
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::readTitle
 * @param item
 */
void XbelTreeReader::readTitle(QTreeWidgetItem *thisItem)
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("title"));
    thisItem->setText(0, myXml.readElementText());
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::readSeparator
 * @param item
 */
void XbelTreeReader::readSeparator(QTreeWidgetItem *thisItem)
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("separator"));
    if (thisItem)
    {
        QTreeWidgetItem *separator = createChildItem(thisItem);
        separator->setFlags(thisItem->flags() & ~Qt::ItemIsSelectable);
        separator->setText(0, QString(30, u'\xB7'));
        myXml.skipCurrentElement();
    }
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::readFolder
 * @param item
 */
void XbelTreeReader::readFolder(QTreeWidgetItem *thisItem)
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("folder"));

    QTreeWidgetItem *theFolder = createChildItem(thisItem);
    bool isFolded = (myXml.attributes().value(foldedElement()) != QLatin1String("no"));
    theFolder->setExpanded(!isFolded);
    theFolder->setIcon(0, myFolderIcon);

    while (myXml.readNextStartElement())
    {
             if (myXml.name() == QLatin1String("title"))     { readTitle(theFolder); }
        else if (myXml.name() == QLatin1String("folder"))    { readFolder(theFolder); }
        else if (myXml.name() == QLatin1String("bookmark"))  { readBookmark(theFolder); }
        else if (myXml.name() == QLatin1String("separator")) { readSeparator(theFolder); }
        else                                                 { myXml.skipCurrentElement(); }
    }
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::readBookmark
 * @param item
 * <bookmark href="http://url.tdl/">
 *      <title>Title</title>
 * </bookmark>
 */
void XbelTreeReader::readBookmark(QTreeWidgetItem *thisItem)
{
    Q_ASSERT(myXml.isStartElement() && myXml.name() == QLatin1String("bookmark"));

    QTreeWidgetItem *bookmark = createChildItem(thisItem);
    bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
    bookmark->setIcon(0, myBookmarkIcon);
    bookmark->setText(0, QObject::tr("Unknown title"));
    bookmark->setText(1, myXml.attributes().value(hrefAttribute()).toString());

    while (myXml.readNextStartElement())
    {
        if (myXml.name() == QLatin1String("title")) { readTitle(bookmark); }
        else                                        { myXml.skipCurrentElement(); }
    }
}
/*****************************************************************************/
/**
 * @brief XbelTreeReader::createChildItem
 * @param item
 * @return
 */
QTreeWidgetItem *XbelTreeReader::createChildItem(QTreeWidgetItem *thisItem)
{
    QTreeWidgetItem *childItem;
    if (thisItem)  { childItem = new QTreeWidgetItem(thisItem); }
    else           { childItem = new QTreeWidgetItem(myTreeWidget); }
    childItem->setData(0, Qt::UserRole, myXml.name().toString());
    return childItem;
}
/******************************* End of File *********************************/
