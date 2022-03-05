#ifndef XBELMENUREADER_H
#define XBELMENUREADER_H

#include <QtWidgets>
#include <QIcon>
#include <QXmlStreamReader>
#include <QMenu>
#include <QAction>
#include <QObject>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

/*****************************************************************************/
/**
 * @brief The XbelReader class
 */
class XbelMenuReader : public QObject
{
        Q_OBJECT
    public:
        //! Constructor
        explicit XbelMenuReader(QMenu *thisMenuWidget = nullptr, QObject *parent = nullptr);
        ~XbelMenuReader();
        // Read XML File
        bool readXmlFile(QIODevice *device);
        // Reported Errors
        QString errorString() const;
        // Names of XML fields
        static inline QString versionAttribute() { return QStringLiteral("version"); }
        static inline QString hrefAttribute()    { return QStringLiteral("href"); }
        static inline QString foldedAttribute()  { return QStringLiteral("folded"); }

    public slots:
        void onOpenBookmark(const QString &thisUrl);

    signals:
        void handleOpenBookmark(const QString &thisUrl);

    private:
        // Functions
        void readXBEL();
        void readTitle();
        void readSeparator();
        void readFolder();
        void readBookmark();
        // Variables
        QXmlStreamReader myXml;
        QIcon            myFolderIcon;
        QIcon            myBookmarkIcon;
        QMenu           *myMenuWidget;
        QAction         *myAction;
        QMenu           *mySubMenu;
        QMenu           *myTopSubMenu;
        QString          myTitle         = "";
        QString          myUrl           = "";
        bool             isFolder        = false;
        bool             isFolderChanged = true;
        bool             isFolderTitle   = false;
        bool             isSeparator     = false;

}; // end class XbelReader
#endif // end XBELMENUREADER_H
/******************************* End of File *********************************/
