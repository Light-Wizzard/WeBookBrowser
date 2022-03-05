#ifndef MYSYSTEMINFORMATION_H
#define MYSYSTEMINFORMATION_H

#include <QObject>
#include <QSysInfo>
#include <QStorageInfo>
#include <QSettings>
#include <QProcess>
#include <QHostInfo>
#include <QtCore>
#include <QtDebug>
#include <QtGlobal>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkInterface>
#include <QTime>
#ifndef MY_QML
    #include <QApplication>
    //#include <QDesktopWidget>
    #include <QGuiApplication>
    #include <QScreen>
#endif


QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

/*****************************************************************************/
/**
 * @brief The MySystemInformation class
 */
class MySystemInformation : public QObject
{
        Q_OBJECT
    public:
        explicit MySystemInformation(QObject *parent = nullptr); //!< My System Information Construtor

        QString runCommandWithReturn(const QString &thisTitle, const QString &thisCommand, const QStringList &thisArguments);
        QString getCpuArchitecture();                           //!< x86_64
        QString getProductName();                               //!< Linux, Mac, Windows 10
        QString getProductType();                               //!< get Product Type
        QString getProductVersion();                            //!< get Product Version
        QString getKernelType();                                //!< linux, mac, winnt
        QString getKernelVersion();                             //!< get Kernel Version
        QString getMachineHostName();                           //!< get Machine Host Name
        QStringList getStorageInfo();                           //!< get Storage Info
        QStringList getNetworkInfo();                           //!< get Network Info
        QStringList getBiosInfo();                              //!< get Bios Info
        QString getCpuName();                                   //!< get Cpu Name
        QString getGpuName();                                   //!< get Gpu Name
        QString getGpuRam();                                    //!< get Gpu Ram
        QString getPrettyDouble(double thisUglyDouble);         //!< get Pretty Double
        QString translateDouble(const QString &thisDouble);     //!< translate Double
        QLocale localeLanguage(const QString &thisLocaleCode);  //!< locale Language
        QString getLanguageCode();                              //!< get Language Code
        void setLanguageCode(const QString &thisLanguageCode);  //!< set Language Code
        QString getDefaultLanguageCode();                       //!< get Default Language Code
        QString language(const QLocale &thisLocale);            //!< language
        QString localMachineName();                             //!< local Machine Name
        QString getPublicIp();                                  //!< get Public Ip
        QString getHtml(const QString &thisUrl);                //!< get Html
        QString getScreen();                                    //!< get Screen
        QString getPublicIpUrl(const QLocale &thisLocale);      //!< get Public Ip Url

        const QString getMacAddress(); //!< get Mac Address
        const QString getIpAddress();  //!< get Ip Address

        QByteArray downloadFromUrl(const QString &thisUrl);  //!< download From Url
        QString getUserName();  //!< get User Name
        QString getUserHome();  //!< get User Home
        QString getEnvironmentVar(const QString &thisVar, const QString &thisDefault); //!< get Environment Var
        //
        QVector<QNetworkReply *> currentDownloads;
        static bool isHttpRedirect(QNetworkReply *reply);
        void doDownload(const QUrl &thisUrl);
        bool saveToDisk(const QString &filename, QIODevice *data);
        static QString saveFileName(const QUrl &theUrl);

        void loginWebsite(const QString &thisWebsite, const QString &thisLoginSite, const QString &thisUserName, const QString &thisPassword);
        bool isInternet();

    public slots:
        void serviceRequestFinished(QNetworkReply *reply);
        void downloadFinished(QNetworkReply *thisReply);
        void sslErrors(const QList<QSslError> &errors);

    private:
        QString                myLanguageCode          = "";           //!< \c myLanguageCode          @brief Two Digit Language Code
        QString                myPublicIpUrl           = "";
        QNetworkAccessManager *myNetworkManager;
        QUrl                   myDownloadUrl;
        QStringList            myMessagingQue;
        bool                   isDiskSave  = false;
        QStringList            myRequestQue;

}; // end class MySystemInformation
#endif // MYSYSTEMINFORMATION_H
/******************************* End of File *********************************/
