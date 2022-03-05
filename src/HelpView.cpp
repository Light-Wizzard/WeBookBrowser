#include "HelpView.h"

/*****************************************************************************/
/**
 * @brief HelpView::HelpView
 * @param parent
 */
HelpView::HelpView(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    verticalLayoutHelp = new QVBoxLayout(this);
    verticalLayoutHelp->setObjectName(QString::fromUtf8("verticalLayoutHelp"));
    myTextBrowser = new QTextBrowser(this);
    myTextBrowser->setObjectName(QString::fromUtf8("myTextBrowser"));

    verticalLayoutHelp->addWidget(myTextBrowser);

}
/*****************************************************************************/
/**
 * @brief HelpView::setPageSource
 * @param thisPageSource
 */
void HelpView::setPageSource(const QString &thisPageSource)
{
    if (myPageSource != thisPageSource)
    {
        myPageSource = thisPageSource;
        QUrl myQUrl = QUrl(thisPageSource);
        if (myQUrl.isValid())
        {
            myTextBrowser->setSource(myQUrl);
        }
    }
}
/*****************************************************************************/
/**
 * @brief HelpView::getPageSource
 * @return
 */
QString HelpView::getPageSource()
{
    return myPageSource;
}
/*****************************************************************************/
/**
 * @brief HelpView::favIcon
 * @return
 */
QIcon HelpView::favIcon() const
{
    // FIXME icon
    static QIcon favIcon(QStringLiteral(":help.png"));
    return favIcon;
}
/******************************* End of File *********************************/
