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

#include "DownloadManagerWidget.h"
#include "MainWindow.h"

/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::DownloadManagerWidget
 * @param thisTableWidgetDownloads
 * @param parent
 */
DownloadManagerWidget::DownloadManagerWidget(QTableWidget *thisTableWidgetDownloads, QObject *parent) : myTableWidgetDownloads(thisTableWidgetDownloads)
{
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::downloadRequested
 * @param thisDownload
 */
void DownloadManagerWidget::downloadRequested(QWebEngineDownloadRequest *thisDownload)
{
    Q_ASSERT(thisDownload && thisDownload->state() == QWebEngineDownloadRequest::DownloadRequested);

    QString path = QFileDialog::getSaveFileName(nullptr, tr("Save as"), QDir(thisDownload->downloadDirectory()).filePath(thisDownload->downloadFileName()));
    if (path.isEmpty()) { return; }

    thisDownload->setDownloadDirectory(QFileInfo(path).path());
    thisDownload->setDownloadFileName(QFileInfo(path).fileName());
    thisDownload->accept();
    add(thisDownload);
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::add
 * @param downloadWidget
 */
void DownloadManagerWidget::add(QWebEngineDownloadRequest *thisDownload)
{
    int theDownloadNum = myNumDownloads;
    myDownload.append(thisDownload);
    QElapsedTimer theElapsedTimer;
    theElapsedTimer.start();
    myTimeAdded.append(theElapsedTimer);
    //
    myTableWidgetDownloads->insertRow(theDownloadNum);
    // URL with File Name
    QLabel *labelUrl = new QLabel();
    labelUrl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    QString theUrl = thisDownload->url().toDisplayString();
    labelUrl->setText(theUrl.trimmed());
    labelUrl->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    myTableWidgetDownloads->setCellWidget(myNumDownloads, CELL_URL, labelUrl);
    // Timer
    QLabel *labelTimer = new QLabel();
    labelTimer->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    labelTimer->setText("00:00:00");
    labelTimer->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    myTableWidgetDownloads->setCellWidget(myNumDownloads, CELL_TIMER, labelTimer);

    QPushButton *thePushButton = new QPushButton();
    static QIcon cancelIcon(QStringLiteral(":process-stop.png"));
    thePushButton->setIcon(cancelIcon);
    thePushButton->setToolTip(tr("Stop Downloading"));

    connect(thePushButton, &QPushButton::clicked, this, [this, theDownloadNum]() { remove(theDownloadNum); });
    //connect(myCancelButton.at(myNumDownloads), &QPushButton::clicked, this, &DownloadManagerWidget::remove);

    myTableWidgetDownloads->setCellWidget(myNumDownloads, CELL_CANCEL, thePushButton);


    connect(myDownload.at(theDownloadNum), &QWebEngineDownloadRequest::receivedBytesChanged, this, [this, theDownloadNum]() { updateWidget(theDownloadNum); });

    //connect(myDownload.at(theDownloadNum), &QWebEngineDownloadRequest::stateChanged,     this, &DownloadManagerWidget::updateWidget);
    connect(myDownload.at(theDownloadNum), &QWebEngineDownloadRequest::stateChanged, this, [this, theDownloadNum]() { updateWidget(theDownloadNum); });

    QProgressBar *theProgressBar = new QProgressBar();
    theProgressBar->setValue(0);
    theProgressBar->setDisabled(false);
    theProgressBar->setFormat(tr("%p% - %1 of %2 downloaded - %3/s").arg(withUnit(0), withUnit(0), withUnit(0)));
    myTableWidgetDownloads->setCellWidget(myNumDownloads, CELL_PROGRESS, theProgressBar);



    myNumDownloads++;
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::updateWidget
 */
void DownloadManagerWidget::updateWidget(int thisWidget)
{
    qreal totalBytes = myDownload.at(thisWidget)->totalBytes();
    qreal receivedBytes = myDownload.at(thisWidget)->receivedBytes();
    qreal bytesPerSecond = receivedBytes / myTimeAdded.at(thisWidget).elapsed() * 1000;

    auto state = myDownload.at(thisWidget)->state();
    QProgressBar *theProgressBar = qobject_cast<QProgressBar*>(myTableWidgetDownloads->cellWidget(thisWidget, CELL_PROGRESS));
    QLabel *theTimerLable = qobject_cast<QLabel*>(myTableWidgetDownloads->cellWidget(thisWidget, CELL_TIMER));
    theTimerLable->setText(QDateTime::fromMSecsSinceEpoch(myTimeAdded.at(thisWidget).elapsed()).toUTC().toString("hh:mm:ss"));
    // State
    switch (state)
    {
        case QWebEngineDownloadRequest::DownloadRequested:
            Q_UNREACHABLE();
            break;
        case QWebEngineDownloadRequest::DownloadInProgress:
            if (totalBytes >= 0)
            {
                theProgressBar->setValue(qRound(100 * receivedBytes / totalBytes));
                theProgressBar->setDisabled(false);
                theProgressBar->setFormat(tr("%p% - %1 of %2 downloaded - %3/s").arg(withUnit(receivedBytes), withUnit(totalBytes), withUnit(bytesPerSecond)));
            }
            else
            {
                theProgressBar->setValue(0);
                theProgressBar->setDisabled(false);
                theProgressBar->setFormat(tr("unknown size - %1 downloaded - %2/s").arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
            }
            break;
        case QWebEngineDownloadRequest::DownloadCompleted:
            theProgressBar->setValue(100);
            theProgressBar->setDisabled(true);
            theProgressBar->setFormat(tr("completed - %1 downloaded - %2/s").arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
            break;
        case QWebEngineDownloadRequest::DownloadCancelled:
            theProgressBar->setValue(0);
            theProgressBar->setDisabled(true);
            theProgressBar->setFormat(tr("cancelled - %1 downloaded - %2/s").arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
            break;
        case QWebEngineDownloadRequest::DownloadInterrupted:
            theProgressBar->setValue(0);
            theProgressBar->setDisabled(true);
            theProgressBar->setFormat(tr("interrupted: %1").arg(myDownload.at(thisWidget)->interruptReasonString()));
            break;
    }
    QPushButton *theCancelButton = qobject_cast<QPushButton*>(myTableWidgetDownloads->cellWidget(thisWidget, CELL_CANCEL));
    // FIXME remove on second click
    if (state == QWebEngineDownloadRequest::DownloadInProgress)
    {
        static QIcon cancelIcon(QStringLiteral(":process-stop.png"));
        theCancelButton->setIcon(cancelIcon);
        theCancelButton->setToolTip(tr("Stop downloading"));
    }
    else
    {
        static QIcon removeIcon(QStringLiteral(":edit-clear.png"));
        theCancelButton->setIcon(removeIcon);
        theCancelButton->setToolTip(tr("Remove from list"));
    }
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::remove
 * @param downloadWidget
 */
void DownloadManagerWidget::remove(int thisDownload)
{
    if (!myDownload.at(thisDownload)->isFinished())
    {
        myDownload.at(thisDownload)->cancel();
    }
    myTableWidgetDownloads->removeRow(thisDownload);
    myDownload.removeAt(thisDownload);
    myTimeAdded.removeAt(thisDownload);
    --myNumDownloads;
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::favIcon
 * @return
 */
QIcon DownloadManagerWidget::favIcon() const
{
    // FIXME icon
    static QIcon theFavIcon(QStringLiteral(":go-bottom.png"));
    return theFavIcon;
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::rowCount
 * @return
 */
int DownloadManagerWidget::rowCount(const QModelIndex & /*parent*/) const
{
    return myDownload.length();
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::columnCount
 * @return
 */
int DownloadManagerWidget::columnCount(const QModelIndex & /*parent*/) const
{
    return 4;
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::data
 * @param thisIndex
 * @param thisRole
 * @return
 */
QVariant DownloadManagerWidget::data(const QModelIndex &thisIndex, int thisRole) const
{
    int row = thisIndex.row();
    int col = thisIndex.column();
    // generate a log message when this method gets called
    qDebug() << QString("row %1, col%2, role %3").arg(row, col, thisRole);

    switch (thisRole)
    {
        case Qt::DisplayRole:
            {
                if (thisIndex.column() == CELL_URL)
                {
                    QLabel *theUrl = qobject_cast<QLabel*>(myTableWidgetDownloads->cellWidget(thisIndex.row(), CELL_URL));
                    return QVariant::fromValue(theUrl);
                }
                else if (thisIndex.column() == CELL_PROGRESS)
                {
                    QProgressBar *theProgressBar = qobject_cast<QProgressBar*>(myTableWidgetDownloads->cellWidget(thisIndex.row(), CELL_PROGRESS));
                    return QVariant::fromValue(theProgressBar);
                }
                else if (thisIndex.column() == CELL_CANCEL)
                {
                    QPushButton *theCancelButton = qobject_cast<QPushButton*>(myTableWidgetDownloads->cellWidget(thisIndex.row(), CELL_CANCEL));
                    return QVariant::fromValue(theCancelButton);
                }
                else if (thisIndex.column() == CELL_TIMER)
                {
                    QLabel *theTimerLable = qobject_cast<QLabel*>(myTableWidgetDownloads->cellWidget(thisIndex.row(), CELL_TIMER));
                    return QVariant::fromValue(theTimerLable);
                }
                return QString("Row%1, Column%2").arg(row + 1, col +1);
            }
        case Qt::FontRole:
            // change font only for cell(x,0)
            if (col == 0)
            {
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }
            break;
        case Qt::BackgroundRole:
            //change background only for cell(x,1)
            if (col == 1) { return QBrush(Qt::lightGray); }
            break;
        case Qt::TextAlignmentRole:
            // change text alignment only for cell(x,3)
            if (col == 3) { return int(Qt::AlignRight | Qt::AlignVCenter); }
            break;
            //case Qt::CheckStateRole:
            // add a checkbox to cell(1,0)
            //if (row == 1 && col == 0) { return Qt::Checked; }
            //break;
    }
    return QVariant();
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::headerData
 * @param thisSection
 * @param thisOrientation
 * @param thisRole
 * @return
 */
QVariant DownloadManagerWidget::headerData(int thisSection, Qt::Orientation thisOrientation, int thisRole) const
{
    if (thisRole == Qt::DisplayRole && thisOrientation == Qt::Horizontal)
    {
        switch (thisSection)
        {
            case 0: return QString("URL");
            case 1: return QString("Timer");
            case 2: return QString("Progress");
            case 3: return QString("Cancel");
        }
    }
    return QVariant();
}
/*****************************************************************************/
/**
 * @brief DownloadManagerWidget::withUnit
 * @param bytes
 * @return
 */
inline QString DownloadManagerWidget::withUnit(qreal bytes)
{
         if (bytes < (1 << 10)) { return tr("%L1 B").arg(bytes); }
    else if (bytes < (1 << 20)) { return tr("%L1 KiB").arg(bytes / (1 << 10), 0, 'f', 2); }
    else if (bytes < (1 << 30)) { return tr("%L1 MiB").arg(bytes / (1 << 20), 0, 'f', 2); }
    else                        { return tr("%L1 GiB").arg(bytes / (1 << 30), 0, 'f', 2); }
}
/******************************* End of File *********************************/
