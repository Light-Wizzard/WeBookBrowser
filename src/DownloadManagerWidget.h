/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef DOWNLOADMANAGERWIDGET_H
#define DOWNLOADMANAGERWIDGET_H


#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <QWebEngineDownloadRequest>
#include <QTableWidget>
#include <QDebug>
#include <QPushButton>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QLabel>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

/*****************************************************************************/
/**
 * @brief The DownloadManagerWidget class Displays a list of downloads.
 */
class DownloadManagerWidget : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit DownloadManagerWidget(QTableWidget *thisTableWidgetDownloads, QObject *parent = nullptr);
        /*!
            \enum MyTableCells
            @brief Cell number of Table
         */
        enum MyTableCells
        {
            CELL_URL      = 0, //!< \c CELL_URL      @brief Url of Download
            CELL_TIMER    = 1, //!< \c CELL_TIMER    @brief Timer
            CELL_PROGRESS = 2, //!< \c CELL_PROGRESS @brief Progressbar
            CELL_CANCEL   = 3, //!< \c CELL_CANCEL   @brief Cancel Button
            CELL_COUNT    = 4  //!< \c CELL_COUNT    @brief Cell Count
        }; // end enum MyFileinfo
        // Makes working with cells easier
        Q_ENUM(MyTableCells)
        /*
         * Prompts user with a "Save As" dialog.
         * If the user doesn't cancel it,
         * then the QWebEngineDownloadRequest will be accepted and the DownloadManagerWidget will be shown on the screen.
         */
        void downloadRequested(QWebEngineDownloadRequest *thisDownload);

        QIcon favIcon() const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &thisIndex, int thisRole = Qt::DisplayRole) const override;
        QVariant headerData(int thisSection, Qt::Orientation thisOrientation, int thisRole) const override;

    private slots:
        void updateWidget(int thisWidget);

    private:
        void add(QWebEngineDownloadRequest *thisDownload);
        void remove(int thisDownload);
        QString withUnit(qreal bytes);
        // URL
        QTableWidget *myTableWidgetDownloads;
        QList<QWebEngineDownloadRequest*> myDownload;
        QList<QElapsedTimer>              myTimeAdded;
        //
        int         myNumDownloads = 0;
        // Name
        //QList<QString> myDownloadFileDstName;
        //QList<QString> myDownloadSrcUrl;
        // Progress
        //QList<QProgressBar*>myProgressBar;
        // Cancel
        //QList<QPushButton*>myCancelButton;
        //

}; // end class DownloadManagerWidget
#endif // DOWNLOADMANAGERWIDGET_H
/******************************* End of File *********************************/
