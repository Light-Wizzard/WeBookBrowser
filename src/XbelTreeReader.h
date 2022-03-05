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

#ifndef XbelTreeReader_H
#define XbelTreeReader_H

#include <QIcon>
#include <QXmlStreamReader>
#include <QtWidgets>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

/*****************************************************************************/
/**
 * @brief The XbelTreeReader class
 */
class XbelTreeReader
{
    public:
        //! Constructor
        XbelTreeReader(QTreeWidget *thisTreeWidget);

        bool read(QIODevice *thisDevice);

        QString errorString() const;

        static inline QLatin1String xbelElement()      { return QLatin1String("xbel"); }
        static inline QLatin1String xbelVersion()      { return QLatin1String("1.0"); }
        static inline QLatin1String titleElement()     { return QLatin1String("title"); }
        static inline QLatin1String separatorElement() { return QLatin1String("separator"); }
        static inline QLatin1String foldedElement()    { return QLatin1String("folded"); }
        static inline QLatin1String bookmarkElement()  { return QLatin1String("bookmark"); }
        static inline QString versionAttribute()       { return QStringLiteral("version"); }
        static inline QString hrefAttribute()          { return QStringLiteral("href"); }
        static inline QString yesValue()               { return QStringLiteral("yes"); }
        static inline QString noValue()                { return QStringLiteral("no"); }

    private:
        void readXBEL();
        void readTitle(QTreeWidgetItem *thisItem);
        void readSeparator(QTreeWidgetItem *thisItem);
        void readFolder(QTreeWidgetItem *thisItem);
        void readBookmark(QTreeWidgetItem *thisItem);
        QTreeWidgetItem *createChildItem(QTreeWidgetItem *thisItem);
        //
        QTreeWidget     *myTreeWidget;
        QXmlStreamReader myXml;

        QIcon           myFolderIcon;
        QIcon           myBookmarkIcon;
}; // end class XbelTreeReader
#endif
/******************************* End of File *********************************/
