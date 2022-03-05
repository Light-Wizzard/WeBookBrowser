#include "MySystemInformation.h"
/*****************************************************************************/
/**
 * @brief MySystemInformation::MySystemInformation
 * @param parent
 */
MySystemInformation::MySystemInformation(QObject *parent) : QObject(parent)
{
    myNetworkManager = new QNetworkAccessManager(this);
    connect(myNetworkManager, &QNetworkAccessManager::finished, this, &MySystemInformation::downloadFinished);

}
/*****************************************************************************/
/**
 * @brief MySystemInformation::runCommandWithReturn
 * @param thisTitle
 * @param thisCommand
 * @param thisArguments
 * @return
 */
QString MySystemInformation::runCommandWithReturn(const QString &thisTitle, const QString &thisCommand, const QStringList &thisArguments)
{
    QProcess theSystemProcess;
    QString theSystemOutput;
    QString theReturnValue;
    theSystemProcess.start(thisCommand, thisArguments);
    theSystemProcess.waitForFinished();
    theSystemOutput = theSystemProcess.readAllStandardOutput();
    theReturnValue.append(QString("%1: %2").arg(thisTitle, theSystemOutput.toLocal8Bit().constData()));
    return theReturnValue;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getCpuArchitecture x86_64
 * @return QSysInfo::currentCpuArchitecture().toLocal8Bit().constData()
 */
QString MySystemInformation::getCpuArchitecture()
{
    return QSysInfo::currentCpuArchitecture().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getProductName Linux, Mac, Windows 10
 * @return QSysInfo::prettyProductName().toLocal8Bit().constData()
 */
QString MySystemInformation::getProductName()
{
    return QSysInfo::prettyProductName().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getProductType
 * @return
 */
QString MySystemInformation::getProductType()
{
    return QSysInfo::productType().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getProductVersion
 * @return
 */
QString MySystemInformation::getProductVersion()
{
    return QSysInfo::productVersion().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getKernelType linux, mac, winnt
 * @return QSysInfo::kernelType().toLocal8Bit().constData()
 */
QString MySystemInformation::getKernelType()
{
    return QSysInfo::kernelType().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getKernelVersion
 * @return
 */
QString MySystemInformation::getKernelVersion()
{
    return QSysInfo::kernelVersion().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getMachineHostName
 * @return
 */
QString MySystemInformation::getMachineHostName()
{
    return QSysInfo::machineHostName().toLocal8Bit().constData();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getStorageInfo
 * @return
 */
QStringList MySystemInformation::getStorageInfo()
{
    QStringList theStorageInfo;
    QString theStorageInfoLine;
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes())
    {
        theStorageInfoLine.clear();
        if (storage.isValid() && storage.isReady())
        {
            if (!storage.isReadOnly())
            {
                theStorageInfoLine.append(QString("%1: %2").arg(tr("Name"), storage.name().toLocal8Bit().constData()));
                theStorageInfoLine.append(QString(" %1: %2").arg(tr("Root"), storage.rootPath().toLocal8Bit().constData()));
                theStorageInfoLine.append(QString(" %1: %2").arg(tr("FileSys"), storage.fileSystemType().constData()));
                QString theFreeSpace = getPrettyDouble(storage.bytesAvailable());
                QString theSpace = getPrettyDouble(storage.bytesTotal());
                theStorageInfoLine.append(QString(" %1: %2 %3: %4").arg(tr("Disk Spaces"), theSpace, tr("Free Disk Spaces"), theFreeSpace));
                theStorageInfo.append(theStorageInfoLine);
            }
        } // end if (storage.isValid() && storage.isReady())
    } // end foreach
    return theStorageInfo;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getNetworkInfo
 * @return
 */
QStringList MySystemInformation::getNetworkInfo()
{
    QStringList theNetworkInfo;
    QString theNetworkInfoLine;
    foreach(QNetworkInterface networkInterface, QNetworkInterface::allInterfaces())
    {
        if (networkInterface.flags().testFlag(QNetworkInterface::IsUp) && !networkInterface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            foreach (QNetworkAddressEntry entry, networkInterface.addressEntries())
            {
                theNetworkInfoLine.clear();
                if ( entry.ip().toString().contains("."))
                {
                    theNetworkInfoLine.append(QString("%1: %2").arg(tr("Interface"), networkInterface.name().toLocal8Bit().constData()));
                    theNetworkInfoLine.append(QString(" %1: %2").arg(tr("IP"), entry.ip().toString().toLocal8Bit().constData()));
                    theNetworkInfoLine.append(QString(" %1: %2").arg(tr("MAC"), networkInterface.hardwareAddress().toLocal8Bit().constData()));
                    theNetworkInfo.append(theNetworkInfoLine);
                }
            }
        }
    }
    return theNetworkInfo;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getPublicIp
 * @return
 */
QString MySystemInformation::getPublicIp()
{
    QString strIp = getHtml(getPublicIpUrl(localeLanguage(getLanguageCode())));
    if(!strIp.isEmpty())
    {
        if (strIp.contains("="))
        {
            // FIXME
            // This depends on the URL used to get IP address
            QString web = strIp.replace("", "");
            web = web.replace("\r", "");
            web = web.replace("\n", "");
            QStringList list = web.split("<br/>" );
            QString tar = list[3];
            QStringList ip = tar.split("=");
            strIp = ip[1];
        }
    }
    else
    {
        strIp = tr("Unable to obtain public network ip");
    }
    return strIp;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getHtml
 * @param thisUrl
 * @return
 */
QString MySystemInformation::getHtml(const QString &thisUrl)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(thisUrl)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    // FIXME
    connect(manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    //connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    //connect(timer, SIGNAL(timeout() ), &eventLoop, SLOT(quit()));
    connect(timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    timer->start(300);
    eventLoop.exec();
    responseData = reply->readAll();
    delete timer;
    return QString(responseData);
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::downloadFromUrl
 * const string webpageText = downloadFromUrl(url).toStdString();
 * @param thisUrl
 * @return theResponse
 */
QByteArray MySystemInformation::downloadFromUrl(const QString &thisUrl)
{
    QNetworkAccessManager *theNetworkManager = new QNetworkAccessManager();
    QNetworkReply *theResponse = theNetworkManager->get(QNetworkRequest(QUrl(thisUrl)));
    QEventLoop theEventLoop;
    QObject::connect(theResponse, &QNetworkReply::finished, &theEventLoop, &QEventLoop::quit);
    theEventLoop.exec();
    return theResponse->readAll();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getUserName
 * @return
 */
QString MySystemInformation::getUserName()
{
    QString theUserName = qgetenv("USER");
    if (theUserName.isEmpty()) { theUserName = qgetenv("USERNAME"); }
    if (theUserName.isEmpty())
    {
        theUserName = getEnvironmentVar("USER", "");
        if (theUserName.isEmpty()) { theUserName = getEnvironmentVar("USERNAME", ""); }
    }
    if (theUserName.isEmpty()) { theUserName = getUserHome(); }
    return theUserName;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getUserHome
 * @return
 */
QString MySystemInformation::getUserHome()
{
    return QDir::home().dirName();
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getEnvironmentVar
 * inline auto get_environement_variable(const char* key )
 * {
 *     char * val = getenv(key);
 *     return (val == NULL) ? "" : std::string(val);
 * }
 * auto programFiles = get_environement_variable("%ProgramW6432%");
 * auto programFilesX86 = get_environement_variable("%ProgramFiles(x86)%");
 * auto userProfile = get_environement_variable("%USERPROFILE%");
 * auto userProfile = get_environement_variable("%USERPROFILE%\Documents");
 * @param thisVar
 * @param thisDefault
 * @return
 */
QString MySystemInformation::getEnvironmentVar(const QString &thisVar, const QString &thisDefault)
{
    //setMessage("get Environment Var", QString("getEnvironmentVar(%1)").arg(thisVar), Debug);
    QString theEnvValue = qEnvironmentVariable(thisVar.toLocal8Bit());
    if (!theEnvValue.isNull())
        { return theEnvValue; }
    else
        { return thisDefault; }
}
/*****************************************************************************/

void MySystemInformation::loginWebsite(const QString &thisWebsite, const QString &thisLoginSite, const QString &thisUserName, const QString &thisPassword)
{
    QUrl serviceUrl = QUrl(thisWebsite);
    QNetworkAccessManager *myNetworkManager = new QNetworkAccessManager(this);
    connect(myNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)));
    //connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, &MySystemInformation::serviceRequestFinished(QNetworkReply*));

    QUrlQuery postData;
    postData.addQueryItem("username", thisUserName);
    postData.addQueryItem("password", thisPassword);

    QNetworkRequest request(serviceUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    myNetworkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());

    QNetworkCookieJar *cookies = myNetworkManager->cookieJar();

    QEventLoop eventLoop;

    QNetworkAccessManager mgr;
    mgr.setCookieJar(cookies);
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    // FIXME
    QNetworkRequest req((QUrl(thisLoginSite)));
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        //QMessageBox::information(this, "Result", QString(reply->readAll()));
        myMessagingQue.append(QString(reply->readAll()));
    }
}
/*****************************************************************************/
void MySystemInformation::serviceRequestFinished(QNetworkReply *reply)
{
    //QMessageBox::information(this, "Result", QString(reply->readAll()));
    myMessagingQue.append(reply->readAll());
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::downloadFinished
 * @param thisReply
 */
void MySystemInformation::downloadFinished(QNetworkReply *thisReply)
{
    QUrl theUrl = thisReply->url();
    if (thisReply->error())
    {
        //fprintf(stderr, "Download of %s failed: %s\n", theUrl.toEncoded().constData(), qPrintable(thisReply->errorString()));
        myMessagingQue.append(QString("%1 %2 %3: %4").arg(tr("Download of"), tr("failed"), theUrl.toEncoded().constData(), qPrintable(thisReply->errorString())));
    }
    else
    {
        if (isHttpRedirect(thisReply))
        {
            //fputs("Request was redirected.\n", stderr);
            myMessagingQue.append(QString("%1").arg(tr("Request was redirected.")));
        }
        else
        {
            myMessagingQue.append(QString("%1 %2 %3").arg(tr("Download of"), theUrl.toEncoded().constData(), tr("succeeded")));
            myRequestQue.append(thisReply->readAll());
            if (isDiskSave)
            {
                QString filename = saveFileName(theUrl);
                if (saveToDisk(filename, thisReply))
                {
                    //printf("Download of %s succeeded (saved to %s)\n", theUrl.toEncoded().constData(), qPrintable(filename));
                    myMessagingQue.append(QString("%1: %2").arg(tr("saved to"), qPrintable(filename)));
                }
            }
        }
    }

    currentDownloads.removeAll(thisReply);
    thisReply->deleteLater();
    if (currentDownloads.isEmpty())
    {
        // all downloads finished
    }
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::saveFileName
 * @param theUrl
 * @return theBaseName
 */
QString MySystemInformation::saveFileName(const QUrl &theUrl)
{
    QString thePath = theUrl.path();
    QString theBaseName = QFileInfo(thePath).fileName();
    if (theBaseName.isEmpty()) { theBaseName = "download"; }
    if (QFile::exists(theBaseName))
    {
        // already exists, don't overwrite
        int i = 0;
        theBaseName += '.';
        while (QFile::exists(theBaseName + QString::number(i))) { ++i; }
        theBaseName += QString::number(i);
    }
    return theBaseName;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::saveToDisk
 * @param filename
 * @param data
 * @return
 */
bool MySystemInformation::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile theFile(filename);
    if (!theFile.open(QIODevice::WriteOnly))
    {
        fprintf(stderr, "Could not open %s for writing: %s\n", qPrintable(filename), qPrintable(theFile.errorString()));
        return false;
    }
    theFile.write(data->readAll());
    theFile.close();
    return true;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::doDownload
 * @param thisUrl
 */
void MySystemInformation::doDownload(const QUrl &thisUrl)
{
    QNetworkRequest theRequest(thisUrl);
    QNetworkReply *theReply = myNetworkManager->get(theRequest);
    #if QT_CONFIG(ssl)
    connect(theReply, &QNetworkReply::sslErrors, this, &MySystemInformation::sslErrors);
    #endif
    currentDownloads.append(theReply);
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::sslErrors
 * @param sslErrors
 */
void MySystemInformation::sslErrors(const QList<QSslError> &sslErrors)
{
    #if QT_CONFIG(ssl)
    for (const QSslError &error : sslErrors)
        { fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString())); }
    #else
        Q_UNUSED(sslErrors);
    #endif
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::isHttpRedirect
 * @param reply
 * @return
 */
bool MySystemInformation::isHttpRedirect(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 305 || statusCode == 307 || statusCode == 308;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::isInternet
 * @return
 */
bool MySystemInformation::isInternet()
{
    // Can be connected to Baidu IP description can be connected to the Internet
    QTcpSocket tcpClient;
    tcpClient.abort();
    tcpClient.connectToHost("202.108.22.5", 80);
         //If it is not connected in 100 milliseconds, it is judged that it is offline
    return tcpClient.waitForConnected(100);
}
/*****************************************************************************/

QString MySystemInformation::getScreen()
{
    #ifndef MY_QML
    QString theScreenDescribe = "";
    QSize theMaxScreenSize;
    QList<QSize> theScreenSizeList;
    QList <int> theScreenCountList;

    QList <QScreen*> theScreens = QGuiApplication::screens();

    for(int i = 0; i < theScreens.count(); i++)
    {
        QScreen *theCurrentScreen = theScreens.at(i);
        QRect screenRect = theCurrentScreen->availableGeometry();
        QSize size(screenRect.width(), screenRect.height());
        bool bExist = false;
        for(int j = 0; j < theScreenSizeList.length( ); j++)
        {
            QSize existSize = theScreenSizeList.at(j);
            if(size == existSize)
            {
                theScreenCountList[j]++;
                bExist = true;
                break;
            }
        }

        if(!bExist)
        {
            theScreenSizeList.append(size);
            theScreenCountList.append(1);
        }
    }
    theMaxScreenSize = theScreenSizeList.at(0);
    for(int i = 0; i < theScreenSizeList.length(); i++)
    {
        int width = theScreenSizeList.at(i).width();
        int height = theScreenSizeList.at(i).height();
        if(width > theMaxScreenSize.width() && height > theMaxScreenSize.height())
        { theMaxScreenSize = theScreenSizeList .at(i); }
        theScreenDescribe += QString ("(%1pixel x %2pixel) x %3").arg(width).arg(height).arg(theScreenCountList.at(i));
        if( i!= theScreenSizeList.length()-1) { theScreenDescribe += ", "; }
    }
    return theScreenDescribe;
    #endif
    return QString("");
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getBiosInfo
 * @return
 */
QStringList MySystemInformation::getBiosInfo()
{
    QStringList theBiosInfo;
    if(QSysInfo::kernelType() == "winnt")
    {
        QSettings settings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\BIOS", QSettings::NativeFormat);
        theBiosInfo.append(QString("%1: %2").arg(tr("Base Manufacturer"), settings.value("BaseBoardManufacturer", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("Base Product"), settings.value("BaseBoardProduct", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("BIOS Vendor"), settings.value("BIOSVendor", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("BIOS Release Date"), settings.value("BIOSReleaseDate", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("System Manufacturer"), settings.value("SystemManufacturer", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("Product Name"), settings.value("SystemProductName", "0").toString().toLocal8Bit().constData()));
        theBiosInfo.append(QString("%1: %2").arg(tr("System SKU"), settings.value("SystemSKU", "0").toString().toLocal8Bit().constData()));
    }
    return theBiosInfo;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::localMachineName
 * @return
 */
QString MySystemInformation::localMachineName()
{
    QString machineName = QHostInfo::localHostName();
    return machineName;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getCpuName
 * @return
 */
QString MySystemInformation::getCpuName()
{
    QString theCommand;
    QString theReturn;
    QStringList arguments;
    if(QSysInfo::kernelType() == "winnt")
    {
        theCommand = "wmic cpu get name";
        theReturn = runCommandWithReturn(tr("CPU").toLocal8Bit(), theCommand, arguments);
    }
    if(QSysInfo::kernelType() == "linux")
    {
        theCommand = "cat /proc/cpuinfo | grep 'model name' | uniq";
        theReturn = runCommandWithReturn(tr("CPU").toLocal8Bit(), theCommand, arguments);
        // model name      : Intel(R) Xeon(R) CPU E5-2680 v2 @ 2.80GHz
        if (theReturn.contains(":"))
        {
            theReturn = theReturn.mid(theReturn.indexOf(":") + 1);
            //theReturn.remove(tr("model name      : "));
        }
    }
    return theReturn;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getGpuName
 * @return
 */
QString MySystemInformation::getGpuName()
{
    QString theCommand;
    QString theReturn;
    QStringList arguments;

    if(QSysInfo::kernelType() == "winnt")
    {
        theCommand = "wmic PATH Win32_videocontroller get VideoProcessor ";
    }
    if(QSysInfo::kernelType() == "linux")
    {
        // echo $(lspci | grep VGA | cut -d ":" -f3);
        theCommand = "lspci | grep VGA | cut -d \":\" -f3";
    }
    theReturn = runCommandWithReturn(tr("GPU").toLocal8Bit(), theCommand, arguments);
    return theReturn;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getGpuRam
 * @return
 */
QString MySystemInformation::getGpuRam()
{
    QString theCommand;
    QString theReturn;
    QStringList arguments;
    if(QSysInfo::kernelType() == "winnt")
    {
        theCommand = "wmic PATH Win32_VideoController get AdapterRAM";
    }
    if(QSysInfo::kernelType() == "linux")
    {
        // RAM=$(cardid=$(lspci | grep VGA |cut -d " " -f1); lspci -v -s $cardid | grep " prefetchable"| cut -d "=" -f2);
        theCommand = "echo $(cardid=$(lspci | grep VGA |cut -d \" \" -f1); lspci -v -s $cardid | grep \" prefetchable\"| cut -d \"=\" -f2)";
    }
    theReturn = runCommandWithReturn(tr("GPU RAM").toLocal8Bit(), theCommand, arguments);
    return theReturn;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getDefaultLanguageCode
 * @return
 */
QString MySystemInformation::getDefaultLanguageCode()
{
    //setMessage("get Default LanguageCode", QString("getDefaultLanguageCode(%1)").arg(language(QLocale())), MySystemInformation::MyMessageTypes::Debug);
    return language(QLocale());
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getLanguageCode
 * @return
 */
QString MySystemInformation::getLanguageCode()
{
    if (myLanguageCode.isEmpty()) { setLanguageCode(getDefaultLanguageCode()); }
    //setMessage("get Language Code", QString("getLanguageCode(%1)").arg(myLanguageCode), MySystemInformation::MyMessageTypes::Debug);
    return myLanguageCode;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguageCode
 * @param thisLanguageCode
 */
void MySystemInformation::setLanguageCode(const QString &thisLanguageCode)
{
    //setMessage("set Language Code", QString("setLanguageCode(%1)").arg(thisLanguageCode), MySystemInformation::MyMessageTypes::Debug);
    if (myLanguageCode != thisLanguageCode)
    {
        myLanguageCode = thisLanguageCode;
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateDouble
 * @param thisDouble
 * @return
 */
QString MySystemInformation::translateDouble(const QString &thisDouble)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    //setMessage("translateInt", QString("%1 <- translateInt(%2) Code=%3").arg(QString::number(theLocale.toDouble(thisDouble)), thisDouble, getLanguageCode()), MySystemInformation::MyMessageTypes::Debug);
    return QString::number(theLocale.toDouble(thisDouble));
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getPrettyDouble
 * @param thisUglyDouble
 * @return
 */
QString MySystemInformation::getPrettyDouble(double thisUglyDouble)
{
    // /1000000000
    QLocale theLocale(localeLanguage(getLanguageCode()));
    QString theReturn = theLocale.formattedDataSize(thisUglyDouble);
    return theReturn;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getIpAddress
 * @return
 */
const QString MySystemInformation::getIpAddress()
{
    QString ip = "";

    QList<QNetworkInterface> interFaceList = QNetworkInterface::allInterfaces();
    for(int i = 0; i< interFaceList.size(); i++)
    {
        QNetworkInterface interface = interFaceList.at(i);
        if(interface.flags().testFlag(QNetworkInterface::IsRunning))
        {
            QList<QNetworkAddressEntry> entryList = interface.addressEntries();
            foreach(QNetworkAddressEntry entry, entryList)
            {
                if(QAbstractSocket::IPv4Protocol == entry.ip().protocol() && entry.ip() != QHostAddress::LocalHost && entry.ip().toString().startsWith("192.168."))
                {
                    ip = entry.ip().toString();
                    break;
                }
            }
        }
    }

    return ip;
}
/*****************************************************************************/
/**
 * @brief MySystemInformation::getMacAddress
 * @return
 */
const QString MySystemInformation::getMacAddress()
{
    QString theMacAddress;

    QList<QNetworkInterface> theNetworkList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface theNetworkItem, theNetworkList)
    {
        if((QNetworkInterface::IsUp & theNetworkItem.flags()) && (QNetworkInterface::IsRunning & theNetworkItem.flags()))
        {
            if(theMacAddress.isEmpty() || theMacAddress < theNetworkItem.hardwareAddress())
            {
                theMacAddress = theNetworkItem.hardwareAddress();
            }
        }
    }
    return theMacAddress;
}
/******************************************************************************
 * @brief get Public Ip Url is required to make this work Globally,
 * I have not looked up any of the URLs needed for this to work,
 * as such, this is a FIXME.
 * language
 * @param thisLocale QLocale given QLocale() returns Country Code
 *****************************************************************************/
QString MySystemInformation::getPublicIpUrl(const QLocale &thisLocale)
{
    //setMessage("getPublicIpUrl", QString("getPublicIpUrl(%1)").arg(thisLocale.language()), MyOrgSettings::MyMessageTypes::Debug);
    switch (thisLocale.language())
    {
        case QLocale::Afrikaans:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Albanian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Amharic:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Arabic:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Armenian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Azerbaijani:      return QStringLiteral("https://api.ipify.org");
        case QLocale::Bashkir:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Basque:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Belarusian:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Bengali:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Bosnian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Bulgarian:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Catalan:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Chinese:          return QStringLiteral("http://whois.pconline.com.cn/");
        case QLocale::LiteraryChinese:  return QStringLiteral("http://whois.pconline.com.cn/");
        case QLocale::Corsican:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Croatian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Czech:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Cantonese:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Cebuano:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Chickasaw:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Danish:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Dutch:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Esperanto:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Estonian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Finnish:          return QStringLiteral("https://api.ipify.org");
        case QLocale::French:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Frisian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Fijian:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Galician:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Georgian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::German:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Greek:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Gujarati:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Haitian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Hausa:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Hawaiian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Hebrew:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Hindi:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Hungarian:        return QStringLiteral("https://api.ipify.org");
            //case QLocale::HillMari:       return QStringLiteral("https://api.ipify.org");
            //case QLocale::HmongNjua Hmong:return QStringLiteral("https://api.ipify.org");
        case QLocale::Icelandic:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Igbo:             return QStringLiteral("https://api.ipify.org");
        case QLocale::Indonesian:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Irish:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Italian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Japanese:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Javanese:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Kannada:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Kazakh:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Khmer:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Kinyarwanda:      return QStringLiteral("https://api.ipify.org");
        case QLocale::Korean:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Kurdish:          return QStringLiteral("https://api.ipify.org");
            //case QLocale::Klingon:        return QStringLiteral("https://api.ipify.org");
            //case QLocale::KlingonPlqaD:   return QStringLiteral("https://api.ipify.org");
            //case QLocale::Kyrgyzstan:     return QStringLiteral("https://api.ipify.org");
        case QLocale::Lao:              return QStringLiteral("https://api.ipify.org");
        case QLocale::Latin:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Latvian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Lithuanian:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Luxembourgish:    return QStringLiteral("https://api.ipify.org");
            //case QLocale::LevantineArabic:return QStringLiteral("https://api.ipify.org");
            //case QLocale::Mari:           return QStringLiteral("https://api.ipify.org");
            //case QLocale::Myanmar:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Papiamento:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Macedonian:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Malagasy:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Malay:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Malayalam:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Maltese:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Maori:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Marathi:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Mongolian:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Nepali:           return QStringLiteral("https://api.ipify.org");
        case QLocale::NorwegianBokmal:  return QStringLiteral("https://api.ipify.org");
        case QLocale::Oriya:            return QStringLiteral("https://api.ipify.org");
            //case QLocale::QueretaroOtomi: return QStringLiteral("https://api.ipify.org");
        case QLocale::Pashto:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Persian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Polish:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Portuguese:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Punjabi:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Romanian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Russian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Samoan:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Gaelic:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Serbian:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Shona:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Sindhi:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Sinhala:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Slovak:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Slovenian:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Somali:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Spanish:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Sundanese:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Swahili:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Swedish:          return QStringLiteral("https://api.ipify.org");
            //case QLocale::SerbianLatin:   return QStringLiteral("https://api.ipify.org");
            //case QLocale::Sesotho:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Filipino:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Tajik:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Tamil:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Tatar:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Telugu:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Thai:             return QStringLiteral("https://api.ipify.org");
        case QLocale::Turkish:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Turkmen:          return QStringLiteral("https://api.ipify.org");
            //case QLocale::Tagalog:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Tahitian:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Tongan:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Uighur:           return QStringLiteral("https://api.ipify.org");
        case QLocale::Ukrainian:        return QStringLiteral("https://api.ipify.org");
        case QLocale::Urdu:             return QStringLiteral("https://api.ipify.org");
        case QLocale::Uzbek:            return QStringLiteral("https://api.ipify.org");
            //case QLocale::Udmurt:         return QStringLiteral("https://api.ipify.org");
        case QLocale::Vietnamese:       return QStringLiteral("https://api.ipify.org");
        case QLocale::Welsh:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Xhosa:            return QStringLiteral("https://api.ipify.org");
        case QLocale::Yiddish:          return QStringLiteral("https://api.ipify.org");
        case QLocale::Yoruba:           return QStringLiteral("https://api.ipify.org");
            //case QLocale::YucatecMaya:    return QStringLiteral("https://api.ipify.org");
        case QLocale::Zulu:             return QStringLiteral("https://api.ipify.org");
        default:
            if (thisLocale.name() == "HaitianCreole")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "HillMari")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Hmong")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Klingon")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "KlingonPlqaD")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Kyrgyz")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "LevantineArabic")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Mari")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Myanmar")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Chichewa")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "QueretaroOtomi")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "ScotsGaelic")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "SerbianCyrillic")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "SerbianLatin")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Sesotho")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Tagalog")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "Udmurt")
            { return QStringLiteral("https://api.ipify.org"); }
            else if (thisLocale.name() == "YucatecMaya")
            { return QStringLiteral("https://api.ipify.org"); }
            return QStringLiteral("en");
    }
} // end getPublicIpUrl
/*****************************************************************************/
/**
 * @brief locale Language.
 * localeLanguage
 * @param thisLocaleCode Locale Code returns Country Code
 */
QLocale MySystemInformation::localeLanguage(const QString &thisLocaleCode)
{
    //setMessage("localeLanguage", QString("localeLanguage(%1)").arg(thisLocaleCode), MyOrgSettings::MyMessageTypes::Debug);
    if (thisLocaleCode == "af")         { return QLocale::Afrikaans;   }
    else if (thisLocaleCode == "sq")    { return QLocale::Albanian;    }
    else if (thisLocaleCode == "am")    { return QLocale::Amharic;     }
    else if (thisLocaleCode == "ar")    { return QLocale::Arabic;      }
    else if (thisLocaleCode == "hy")    { return QLocale::Armenian;    }
    else if (thisLocaleCode == "az")    { return QLocale::Azerbaijani; }
    else if (thisLocaleCode == "ba")    { return QLocale::Bashkir;    }
    else if (thisLocaleCode == "eu")    { return QLocale::Basque;     }
    else if (thisLocaleCode == "be")    { return QLocale::Belarusian; }
    else if (thisLocaleCode == "bn")    { return QLocale::Bengali;    }
    else if (thisLocaleCode == "bs")    { return QLocale::Bosnian;    }
    else if (thisLocaleCode == "bg")    { return QLocale::Bulgarian;  }
    else if (thisLocaleCode == "ca")    { return QLocale::Catalan;    }
    else if (thisLocaleCode == "zh-CN") { return QLocale::Chinese;    }
    else if (thisLocaleCode == "zh-TW") { return QLocale::LiteraryChinese; }
    else if (thisLocaleCode == "co")    { return QLocale::Corsican;     }
    else if (thisLocaleCode == "hr")    { return QLocale::Croatian;     }
    else if (thisLocaleCode == "cs")    { return QLocale::Czech;        }
    else if (thisLocaleCode == "yue")   { return QLocale::Cantonese;    }
    else if (thisLocaleCode == "ceb")   { return QLocale::Cebuano;      }
    else if (thisLocaleCode == "ny")    { return QLocale::Chickasaw;    }
    else if (thisLocaleCode == "da")    { return QLocale::Danish;       }
    else if (thisLocaleCode == "nl")    { return QLocale::Dutch;        }
    else if (thisLocaleCode == "eo")    { return QLocale::Esperanto;    }
    else if (thisLocaleCode == "et")    { return QLocale::Estonian;     }
    else if (thisLocaleCode == "fi")    { return QLocale::Finnish;      }
    else if (thisLocaleCode == "fr")    { return QLocale::French;       }
    else if (thisLocaleCode == "fy")    { return QLocale::Frisian;      }
    else if (thisLocaleCode == "fj")    { return QLocale::Fijian;       }
    else if (thisLocaleCode == "gl")    { return QLocale::Galician;     }
    else if (thisLocaleCode == "ka")    { return QLocale::Georgian;     }
    else if (thisLocaleCode == "de")    { return QLocale::German;       }
    else if (thisLocaleCode == "el")    { return QLocale::Greek;        }
    else if (thisLocaleCode == "gu")    { return QLocale::Gujarati;     }
    else if (thisLocaleCode == "ht")    { return QLocale::Haitian;      }
    else if (thisLocaleCode == "ha")    { return QLocale::Hausa;        }
    else if (thisLocaleCode == "haw")   { return QLocale::Hawaiian;     }
    else if (thisLocaleCode == "he")    { return QLocale::Hebrew;       }
    else if (thisLocaleCode == "hi")    { return QLocale::Hindi;        }
    else if (thisLocaleCode == "hu")    { return QLocale::Hungarian;    }
    //else if (thisLocaleCode == "mrj") { return QLocale::HillMari;     }
    //else if (thisLocaleCode == "hmn") { QLocale::HmongNjua return ;   } // Hmong
    else if (thisLocaleCode == "is")    { return QLocale::Icelandic;    }
    else if (thisLocaleCode == "ig")    { return QLocale::Igbo;         }
    else if (thisLocaleCode == "id")    { return QLocale::Indonesian;   }
    else if (thisLocaleCode == "ga")    { return QLocale::Irish;        }
    else if (thisLocaleCode == "it")    { return QLocale::Italian;      }
    else if (thisLocaleCode == "ja")    { return QLocale::Japanese;     }
    else if (thisLocaleCode == "jw")    { return QLocale::Javanese;     }
    else if (thisLocaleCode == "kn")    { return QLocale::Kannada;      }
    else if (thisLocaleCode == "kk")    { return QLocale::Kazakh;       }
    else if (thisLocaleCode == "km")    { return QLocale::Khmer;        }
    else if (thisLocaleCode == "rw")    { return QLocale::Kinyarwanda;  }
    else if (thisLocaleCode == "ko")    { return QLocale::Korean;       }
    else if (thisLocaleCode == "ku")    { return QLocale::Kurdish;      }
    //else if (thisLocaleCode == "tlh")      { return QLocale::Klingon; }
    //else if (thisLocaleCode == "tlh-Qaak") { return QLocale::KlingonPlqaD; }
    //else if (thisLocaleCode == "ky")       { return QLocale::Kyrgyzstan; }
    else if (thisLocaleCode == "la")    { return QLocale::Latin;        }
    else if (thisLocaleCode == "lo")    { return QLocale::Lao;          }
    else if (thisLocaleCode == "lv")    { return QLocale::Latvian;      }
    else if (thisLocaleCode == "lt")    { return QLocale::Lithuanian;   }
    else if (thisLocaleCode == "lb")    { return QLocale::Luxembourgish; }
    //else if (thisLocaleCode == "apc") { return QLocale::LevantineArabic; }
    //else if (thisLocaleCode == "mhr") { return QLocale::Mari;         }
    //else if (thisLocaleCode == "my")  { return QLocale::Myanmar;      }
    else if (thisLocaleCode == "pap")   { return QLocale::Papiamento;   }
    else if (thisLocaleCode == "mk")    { return QLocale::Macedonian;   }
    else if (thisLocaleCode == "mg")    { return QLocale::Malagasy;     }
    else if (thisLocaleCode == "ms")    { return QLocale::Malay;        }
    else if (thisLocaleCode == "ml")    { return QLocale::Malayalam;    }
    else if (thisLocaleCode == "mt")    { return QLocale::Maltese;      }
    else if (thisLocaleCode == "mi")    { return QLocale::Maori;        }
    else if (thisLocaleCode == "mr")    { return QLocale::Marathi;      }
    else if (thisLocaleCode == "mn")    { return QLocale::Mongolian;    }
    else if (thisLocaleCode == "ne")    { return QLocale::Nepali;       }
    else if (thisLocaleCode == "no")    { return QLocale::NorwegianBokmal; }
    else if (thisLocaleCode == "or")    { return QLocale::Oriya;        }
    //else if (thisLocaleCode == "otq") { return QLocale::QueretaroOtomi; }
    else if (thisLocaleCode == "ps")    { return QLocale::Pashto;       }
    else if (thisLocaleCode == "fa")    { return QLocale::Persian;      }
    else if (thisLocaleCode == "pl")    { return QLocale::Polish;       }
    else if (thisLocaleCode == "pt")    { return QLocale::Portuguese;   }
    else if (thisLocaleCode == "pa")    { return QLocale::Punjabi;      }
    else if (thisLocaleCode == "ro")    { return QLocale::Romanian;     }
    else if (thisLocaleCode == "ru")    { return QLocale::Russian;      }
    else if (thisLocaleCode == "sm")    { return QLocale::Samoan;       }
    else if (thisLocaleCode == "gd")    { return QLocale::Gaelic;       }
    else if (thisLocaleCode == "sr")    { return QLocale::Serbian;      }
    else if (thisLocaleCode == "sn")    { return QLocale::Shona;        }
    else if (thisLocaleCode == "sd")    { return QLocale::Sindhi;       }
    else if (thisLocaleCode == "si")    { return QLocale::Sinhala;      }
    else if (thisLocaleCode == "sk")    { return QLocale::Slovak;       }
    else if (thisLocaleCode == "sl")    { return QLocale::Slovenian;    }
    else if (thisLocaleCode == "so")    { return QLocale::Somali;       }
    else if (thisLocaleCode == "es")    { return QLocale::Spanish;      }
    else if (thisLocaleCode == "su")    { return QLocale::Sundanese;    }
    else if (thisLocaleCode == "sw")    { return QLocale::Swahili;      }
    else if (thisLocaleCode == "sv")    { return QLocale::Swedish;      }
    //else if (thisLocaleCode == "sr-Latin") { return QLocale::SerbianLatin; }
    //else if (thisLocaleCode == "st")  { return QLocale::Sesotho;      }
    else if (thisLocaleCode == "fil")   { return QLocale::Filipino;     }
    else if (thisLocaleCode == "tg")    { return QLocale::Tajik;        }
    else if (thisLocaleCode == "ta")    { return QLocale::Tamil;        }
    else if (thisLocaleCode == "tt")    { return QLocale::Tatar;        }
    else if (thisLocaleCode == "te")    { return QLocale::Telugu;       }
    else if (thisLocaleCode == "th")    { return QLocale::Thai;         }
    else if (thisLocaleCode == "tr")    { return QLocale::Turkish;      }
    else if (thisLocaleCode == "tk")    { return QLocale::Turkmen;      }
    //else if (thisLocaleCode == "tl")  { return QLocale::Tagalog;      }
    else if (thisLocaleCode == "ty")    { return QLocale::Tahitian;     }
    else if (thisLocaleCode == "to")    { return QLocale::Tongan;       }
    else if (thisLocaleCode == "ug")    { return QLocale::Uighur;       }
    else if (thisLocaleCode == "uk")    { return QLocale::Ukrainian;    }
    else if (thisLocaleCode == "ur")    { return QLocale::Urdu;         }
    else if (thisLocaleCode == "uz")    { return QLocale::Uzbek;        }
    //else if (thisLocaleCode == "udm") { return QLocale::Udmurt;       }
    else if (thisLocaleCode == "vi")    { return QLocale::Vietnamese;   }
    else if (thisLocaleCode == "cy")    { return QLocale::Welsh;        }
    else if (thisLocaleCode == "xh")    { return QLocale::Xhosa;        }
    else if (thisLocaleCode == "yi")    { return QLocale::Yiddish;      }
    else if (thisLocaleCode == "yo")    { return QLocale::Yoruba;       }
    //else if (thisLocaleCode == "yua") { return QLocale::YucatecMaya;  }
    else if (thisLocaleCode == "zu")    { return QLocale::Zulu;         }
    return QLocale::AnyLanguage;
} // end language(const QLocale
/******************************************************************************
 * @brief language.
 * language
 * @param thisLocale QLocale given QLocale() returns Country Code
 *****************************************************************************/
QString MySystemInformation::language(const QLocale &thisLocale)
{
    //setMessage("language", QString("language(%1)").arg(thisLocale.language()), MyOrgSettings::MyMessageTypes::Debug);
    switch (thisLocale.language())
    {
        case QLocale::Afrikaans:        return QStringLiteral("af");
        case QLocale::Albanian:         return QStringLiteral("sq");
        case QLocale::Amharic:          return QStringLiteral("am");
        case QLocale::Arabic:           return QStringLiteral("ar");
        case QLocale::Armenian:         return QStringLiteral("hy");
        case QLocale::Azerbaijani:      return QStringLiteral("az");
        case QLocale::Bashkir:          return QStringLiteral("ba");
        case QLocale::Basque:           return QStringLiteral("eu");
        case QLocale::Belarusian:       return QStringLiteral("be");
        case QLocale::Bengali:          return QStringLiteral("bn");
        case QLocale::Bosnian:          return QStringLiteral("bs");
        case QLocale::Bulgarian:        return QStringLiteral("bg");
        case QLocale::Catalan:          return QStringLiteral("ca");
        case QLocale::Chinese:          return QStringLiteral("zh-CN");
        case QLocale::LiteraryChinese:  return QStringLiteral("zh-TW");
        case QLocale::Corsican:         return QStringLiteral("co");
        case QLocale::Croatian:         return QStringLiteral("hr");
        case QLocale::Czech:            return QStringLiteral("cs");
        case QLocale::Cantonese:        return QStringLiteral("yue");
        case QLocale::Cebuano:          return QStringLiteral("ceb");
        case QLocale::Chickasaw:        return QStringLiteral("ny");
        case QLocale::Danish:           return QStringLiteral("da");
        case QLocale::Dutch:            return QStringLiteral("nl");
        case QLocale::Esperanto:        return QStringLiteral("eo");
        case QLocale::Estonian:         return QStringLiteral("et");
        case QLocale::Finnish:          return QStringLiteral("fi");
        case QLocale::French:           return QStringLiteral("fr");
        case QLocale::Frisian:          return QStringLiteral("fy");
        case QLocale::Fijian:           return QStringLiteral("fj");
        case QLocale::Galician:         return QStringLiteral("gl");
        case QLocale::Georgian:         return QStringLiteral("ka");
        case QLocale::German:           return QStringLiteral("de");
        case QLocale::Greek:            return QStringLiteral("el");
        case QLocale::Gujarati:         return QStringLiteral("gu");
        case QLocale::Haitian:          return QStringLiteral("ht");
        case QLocale::Hausa:            return QStringLiteral("ha");
        case QLocale::Hawaiian:         return QStringLiteral("haw");
        case QLocale::Hebrew:           return QStringLiteral("he");
        case QLocale::Hindi:            return QStringLiteral("hi");
        case QLocale::Hungarian:        return QStringLiteral("hu");
            //case QLocale::HillMari:       return QStringLiteral("mrj");
            //case QLocale::HmongNjua Hmong:return QStringLiteral("hmn");
        case QLocale::Icelandic:        return QStringLiteral("is");
        case QLocale::Igbo:             return QStringLiteral("ig");
        case QLocale::Indonesian:       return QStringLiteral("id");
        case QLocale::Irish:            return QStringLiteral("ga");
        case QLocale::Italian:          return QStringLiteral("it");
        case QLocale::Japanese:         return QStringLiteral("ja");
        case QLocale::Javanese:         return QStringLiteral("jw");
        case QLocale::Kannada:          return QStringLiteral("kn");
        case QLocale::Kazakh:           return QStringLiteral("kk");
        case QLocale::Khmer:            return QStringLiteral("km");
        case QLocale::Kinyarwanda:      return QStringLiteral("rw");
        case QLocale::Korean:           return QStringLiteral("ko");
        case QLocale::Kurdish:          return QStringLiteral("ku");
            //case QLocale::Klingon:        return QStringLiteral("tlh");
            //case QLocale::KlingonPlqaD:   return QStringLiteral("tlh-Qaak");
            //case QLocale::Kyrgyzstan:     return QStringLiteral("ky");
        case QLocale::Lao:              return QStringLiteral("lo");
        case QLocale::Latin:            return QStringLiteral("la");
        case QLocale::Latvian:          return QStringLiteral("lv");
        case QLocale::Lithuanian:       return QStringLiteral("lt");
        case QLocale::Luxembourgish:    return QStringLiteral("lb");
            //case QLocale::LevantineArabic:return QStringLiteral("apc");
            //case QLocale::Mari:           return QStringLiteral("mhr");
            //case QLocale::Myanmar:        return QStringLiteral("my");
        case QLocale::Papiamento:       return QStringLiteral("pap");
        case QLocale::Macedonian:       return QStringLiteral("mk");
        case QLocale::Malagasy:         return QStringLiteral("mg");
        case QLocale::Malay:            return QStringLiteral("ms");
        case QLocale::Malayalam:        return QStringLiteral("ml");
        case QLocale::Maltese:          return QStringLiteral("mt");
        case QLocale::Maori:            return QStringLiteral("mi");
        case QLocale::Marathi:          return QStringLiteral("mr");
        case QLocale::Mongolian:        return QStringLiteral("mn");
        case QLocale::Nepali:           return QStringLiteral("ne");
        case QLocale::NorwegianBokmal:  return QStringLiteral("no");
        case QLocale::Oriya:            return QStringLiteral("or");
            //case QLocale::QueretaroOtomi: return QStringLiteral("otq");
        case QLocale::Pashto:           return QStringLiteral("ps");
        case QLocale::Persian:          return QStringLiteral("fa");
        case QLocale::Polish:           return QStringLiteral("pl");
        case QLocale::Portuguese:       return QStringLiteral("pt");
        case QLocale::Punjabi:          return QStringLiteral("pa");
        case QLocale::Romanian:         return QStringLiteral("ro");
        case QLocale::Russian:          return QStringLiteral("ru");
        case QLocale::Samoan:           return QStringLiteral("sm");
        case QLocale::Gaelic:           return QStringLiteral("gd");
        case QLocale::Serbian:          return QStringLiteral("sr");
        case QLocale::Shona:            return QStringLiteral("sn");
        case QLocale::Sindhi:           return QStringLiteral("sd");
        case QLocale::Sinhala:          return QStringLiteral("si");
        case QLocale::Slovak:           return QStringLiteral("sk");
        case QLocale::Slovenian:        return QStringLiteral("sl");
        case QLocale::Somali:           return QStringLiteral("so");
        case QLocale::Spanish:          return QStringLiteral("es");
        case QLocale::Sundanese:        return QStringLiteral("su");
        case QLocale::Swahili:          return QStringLiteral("sw");
        case QLocale::Swedish:          return QStringLiteral("sv");
            //case QLocale::SerbianLatin:   return QStringLiteral("sr-Latin");
            //case QLocale::Sesotho:        return QStringLiteral("st");
        case QLocale::Filipino:         return QStringLiteral("fil");
        case QLocale::Tajik:            return QStringLiteral("tg");
        case QLocale::Tamil:            return QStringLiteral("ta");
        case QLocale::Tatar:            return QStringLiteral("tt");
        case QLocale::Telugu:           return QStringLiteral("te");
        case QLocale::Thai:             return QStringLiteral("th");
        case QLocale::Turkish:          return QStringLiteral("tr");
        case QLocale::Turkmen:          return QStringLiteral("tk");
            //case QLocale::Tagalog:        return QStringLiteral("tl");
        case QLocale::Tahitian:         return QStringLiteral("ty");
        case QLocale::Tongan:           return QStringLiteral("to");
        case QLocale::Uighur:           return QStringLiteral("ug");
        case QLocale::Ukrainian:        return QStringLiteral("uk");
        case QLocale::Urdu:             return QStringLiteral("ur");
        case QLocale::Uzbek:            return QStringLiteral("uz");
            //case QLocale::Udmurt:         return QStringLiteral("udm");
        case QLocale::Vietnamese:       return QStringLiteral("vi");
        case QLocale::Welsh:            return QStringLiteral("cy");
        case QLocale::Xhosa:            return QStringLiteral("xh");
        case QLocale::Yiddish:          return QStringLiteral("yi");
        case QLocale::Yoruba:           return QStringLiteral("yo");
            //case QLocale::YucatecMaya:    return QStringLiteral("yua");
        case QLocale::Zulu:             return QStringLiteral("zu");
        default:
            if (thisLocale.name() == "HaitianCreole")
            { return QStringLiteral("ht"); }
            else if (thisLocale.name() == "HillMari")
            { return QStringLiteral("mrj"); }
            else if (thisLocale.name() == "Hmong")
            { return QStringLiteral("hmn"); }
            else if (thisLocale.name() == "Klingon")
            { return QStringLiteral("tlh"); }
            else if (thisLocale.name() == "KlingonPlqaD")
            { return QStringLiteral("tlh-Qaak"); }
            else if (thisLocale.name() == "Kyrgyz")
            { return QStringLiteral("ky"); }
            else if (thisLocale.name() == "LevantineArabic")
            { return QStringLiteral("apc"); }
            else if (thisLocale.name() == "Mari")
            { return QStringLiteral("mhr"); }
            else if (thisLocale.name() == "Myanmar")
            { return QStringLiteral("my"); }
            else if (thisLocale.name() == "ny")
            { return QStringLiteral("Chichewa"); }
            else if (thisLocale.name() == "QueretaroOtomi")
            { return QStringLiteral("otq"); }
            else if (thisLocale.name() == "ScotsGaelic")
            { return QStringLiteral("gd"); }
            else if (thisLocale.name() == "SerbianCyrillic")
            { return QStringLiteral("sr"); }
            else if (thisLocale.name() == "SerbianLatin")
            { return QStringLiteral("sr-Latin"); }
            else if (thisLocale.name() == "Sesotho")
            { return QStringLiteral("st"); }
            else if (thisLocale.name() == "Tagalog")
            { return QStringLiteral("tl"); }
            else if (thisLocale.name() == "Udmurt")
            { return QStringLiteral("udm"); }
            else if (thisLocale.name() == "YucatecMaya")
            { return QStringLiteral("yua"); }
            return QStringLiteral("en");
    }
} // end language(const QLocale
/******************************* End of File *********************************/
