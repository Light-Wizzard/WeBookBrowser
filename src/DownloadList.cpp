#include "DownloadList.h"

/*****************************************************************************/
/**
 * @brief QueueItem::QueueItem
 * @param thisFileName
 * @param thisPath
 */
QueueItem::QueueItem(const QString &thisFileName, const QString &thisPath) : myFileName(thisFileName), myPath(thisPath)
{
    myFileSize = 0;
    myProgress = 0;
    myStatus = PENDING;
}
/*****************************************************************************/
/**
 * @brief ProgressBarDelegate::ProgressBarDelegate
 * @param parent
 */
ProgressBarDelegate::ProgressBarDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}
/*****************************************************************************/
/**
 * @brief ProgressBarDelegate::paint
 * @param painter
 * @param option
 * @param index
 */
void ProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //int progress = index.model()->data().toInt();  // How do I access my models .progress_ property?
    int progress = index.model()->data(index).toString().replace("%", "").toInt();

    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = option.rect;
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.progress = progress;
    progressBarOption.text = QString::number(progress) + "%";
    progressBarOption.textVisible = true;

    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
}
/*****************************************************************************/
/**
 * @brief QueueModel::refresh
 */
void QueueModel::refresh()
{
    emit dataChanged(index(0, 0), index(myData.count()-1, 5));
}
/*****************************************************************************/
/**
 * @brief QueueModel::append
 * @param queueItem
 */
void QueueModel::append(const QueueItem &queueItem)
{
    beginInsertRows(QModelIndex(), myData.count(), myData.count());
    myData.append(queueItem);
    endInsertRows();
}
/*****************************************************************************/
/**
 * @brief QueueModel::headerData
 * @param section
 * @param orientation
 * @param role
 * @return
 */
QVariant QueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) { return QVariant(); }
    if (role != Qt::DisplayRole)       { return QVariant(); }
    switch (section)
    {
        case 0: return "Filename";
        case 1: return "Filesize";
        case 2: return "Progress";
        case 3: return "Status";
        default: return QVariant();
    }
}
/*****************************************************************************/
/**
 * @brief QueueModel::data
 * @param index
 * @param role
 * @return
 */
QVariant QueueModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();
    const QueueItem & queueItem = myData[index.row()];
    switch (index.column())
    {
        case 0: return queueItem.getFilename();
        case 1: return QString::number(queueItem.getFilesize()) + " bytes";
        case 2: return QString::number(queueItem.getProgess()) + "%";
        case 3:
            {
                std::string str;
                switch (queueItem.getStatus())
                {
                    case QueueItem::PENDING :
                        str = "Pending";
                        break;
                    case QueueItem::INPROGRESS:
                        str = "In Progress";
                        break;
                    case QueueItem::FINISHED:
                        str = "Finished";
                        break;
                    case QueueItem::FAILED:
                        str = "Failed";
                        break;

                    default:
                        str = "Unkown";
                }
                return QString::fromStdString(str);
            }
        default: return QVariant();
    };
}
/******************************* End of File *********************************/
