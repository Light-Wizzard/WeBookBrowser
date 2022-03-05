#ifndef DOWNLOADLIST_H
#define DOWNLOADLIST_H

#include <QObject>
#include <QWidget>
#include <QStyledItemDelegate>
#include <QApplication>

/*
https://stackoverflow.com/questions/36889646/qtableview-and-itemdelegate-for-showing-a-progress-bar
 // Queue Table
 ui.tvQueue->setModel(&queueModel_);
 ui.tvQueue->setItemDelegateForColumn(2, new ProgressBarDelegate(this));
*/
/*****************************************************************************/
/**
 * @brief The QueueItem class
 */
class QueueItem
{
    public:
        enum QueueStatus { PENDING = 0, INPROGRESS, FINISHED, FAILED };

    public:
        QueueItem(const QString &thisFileName, const QString &thisPath);
        /*****************************************************************************/
        /**
         * @brief getFilename
         * @return
         */
        QString getFilename() const
        {
            return myFileName;
        }
        /*****************************************************************************/
        /**
         * @brief getLocalPath
         * @return
         */
        QString getLocalPath() const
        {
            return myPath;
        }
        /*****************************************************************************/
        /**
         * @brief setFilesize
         * @param thisFileSize
         */
        void setFilesize(long long thisFileSize)
        {
            if (myFileSize != thisFileSize)
            {
                myFileSize = thisFileSize;
            }
        }
        /*****************************************************************************/
        /**
         * @brief getFilesize
         * @return
         */
        long long getFilesize() const
        {
            return myFileSize;
        }
        /*****************************************************************************/
        /**
         * @brief setProgress
         * @param prg
         */
        void setProgress(int prg)
        {
            if (myProgress != prg)
            {
                myProgress = prg;
            }
        }
        /*****************************************************************************/
        /**
         * @brief getProgess
         * @return
         */
        int getProgess() const
        {
            return myProgress;
        }
        /*****************************************************************************/
        /**
         * @brief setStatus
         * @param status
         */
        void setStatus(QueueItem::QueueStatus status)
        {
            if (myStatus != status)
            {
                myStatus = status;
            }
        }
        /*****************************************************************************/
        /**
         * @brief getStatus
         * @return
         */
        QueueStatus getStatus() const
        {
            return myStatus;
        }

    private:
        QueueStatus myStatus;
        QString     myFileName;
        QString     myPath;
        long long   myFileSize;
        int         myProgress;

}; // end class QueueItem
/*****************************************************************************/
/**
 * @brief The QueueModel class
 */
class QueueModel : public QAbstractTableModel
{
    public:

        explicit QueueModel(QObject * parent = nullptr) : QAbstractTableModel(parent) {}
        int rowCount(const QModelIndex &) const { return myData.count(); }
        int columnCount(const QModelIndex &) const { return 4; }

        QVariant data(const QModelIndex &index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        void append(const QueueItem &queueItem);
        void refresh();

    private:
        QList<QueueItem> myData;

}; // end class QueueModel
/*****************************************************************************/
/**
 * @brief The ProgressBarDelegate class
 */
class ProgressBarDelegate : public QStyledItemDelegate
{
        Q_OBJECT
    public:
        explicit ProgressBarDelegate(QObject *parent = nullptr);
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

}; // end class ProgressBarDelegate
#endif // DOWNLOADLIST_H
/******************************* End of File *********************************/
