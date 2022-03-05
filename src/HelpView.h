#ifndef HELPVIEW_H
#define HELPVIEW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QUrl>
#include <QtWidgets/QVBoxLayout>
#include <Qt>

/*****************************************************************************/
/**
 * @brief The HelpView class
 */
class HelpView : public QWidget
{
        Q_OBJECT
    public:
        explicit HelpView(QWidget *parent = nullptr);

        void setPageSource(const QString &thisPageSource);
        QString getPageSource();

        QIcon favIcon() const;

    private:
        QVBoxLayout    *verticalLayoutHelp;
        QTextBrowser   *myTextBrowser;
        QString         myPageSource;

}; // end class HelpView
#endif // HELPVIEW_H
/******************************* End of File *********************************/
