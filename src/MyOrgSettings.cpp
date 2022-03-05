/************************************************
* @file MyOrgSettings.cpp
* @brief My Orginization Settings for INI files,
* My Org Settings has many functions to help track variables,
* I check variables before setting them to prevent triggers.
* @author Jeffrey Scott Flesher <Jeffrey.Scott.Flesher@>
 ***********************************************/
#include "MyOrgSettings.h"
/*****************************************************************************/
/**
 * @brief MyOrgSettings::MyOrgSettings  Creates QSettings for organizationName, organizationDomain, applicationName.
 * @param parent
 */
MyOrgSettings::MyOrgSettings(QObject *parent) : QObject(parent)
{
    // Fixed random number, change it before using it, then leave it,
    // all data stored with this key will only work with this key.
    // Defined in MyOrgSettings.h
    myCrypto = new QAESEncryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
    myKey = QString("your-string-key");
    myHashKey = QCryptographicHash::hash(myKey.toLocal8Bit(), QCryptographicHash::Sha256);

    //myCrypto->setDebugMessage(isDebugMessage);
    // Trigger for Update
    connect(this, &MyOrgSettings::sendUpdateSettings, this, &MyOrgSettings::onUpdateSettings);
    mySettings = getOrgSettings();
    // Run after GUI loads
    QTimer::singleShot(200, this, &MyOrgSettings::onRunOnStartup);
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::~MyOrgSettings Deconstructor.
 */
MyOrgSettings::~MyOrgSettings()
{

}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::onRunOnStartup
 */
void MyOrgSettings::onRunOnStartup()
{
    if (isDebugMessage)
    {
        setMessage("on Run On Start up", "onRunOnStartup", Debug);
    }
    cryptoTest();
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::cryptoTest
 * @return
 */
bool MyOrgSettings::cryptoTest()
{
    // FIXME move to test project
    QString theTest = "This is my Secret";
    if (theTest != decryptThis(encryptThis(theTest)))
    {
        setMessage(QObject::tr("Crypto Fail").toLocal8Bit(), tr("Crypto Fail: MyOrgSettings::onRunOnStartup()").toLocal8Bit(), Critical);
        return false;
    }
    return true;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getEnvironmentVar
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
QString MyOrgSettings::getEnvironmentVar(const QString &thisVar, const QString &thisDefault)
{
    setMessage("get Environment Var", QString("getEnvironmentVar(%1)").arg(thisVar), Debug);
    QString theEnvValue = qEnvironmentVariable(thisVar.toLocal8Bit());
    if (!theEnvValue.isNull())
        { return theEnvValue; }
    else
        { return thisDefault; }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::onUpdateSettings
 */
void MyOrgSettings::onUpdateSettings()
{
    if (!isOrgSettup) { return; }
    mySettings = getOrgSettings();
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getSettings
 * @return
 */
QSettings *MyOrgSettings::getSettings()
{
    if (mySettings == nullptr)
    {
        setSettings(getOrgSettings());
    }
    return mySettings;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setSettings
 * @param thisSetting
 */
void MyOrgSettings::setSettings(QSettings *thisSetting)
{
    mySettings = thisSetting;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getOrgSettings
 * @return
 */
QSettings *MyOrgSettings::getOrgSettings()
{
    setMessage("get Settings", "getOrgSettings", Debug);
    if (!isFileExists(getIniFullPath()))
    {
        if (!isFileMake(getAppDataLocation(), QString("%1.%2").arg(getIniFileName(), getIniFileExtension()), false))
        {
            setMessage("get Settings", QString("%1: %2").arg(QObject::tr("Failed to make File in getOrgSettings"), getIniFullPath()), Critical);
        }
    }
    return new QSettings(getIniFullPath(), QSettings::IniFormat);
} // end getOrgSettings
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isSetting
 * @param thisFieldName
 * @return
 */
bool MyOrgSettings::isSetting(const QString &thisFieldName)
{
    setMessage("is Setting", QString("isSetting(%1)").arg(thisFieldName), Debug);
    return mySettings->contains(thisFieldName);
} // end isSetting
/*****************************************************************************/
/**
 * @brief MyOrgSettings::readSettings
 * @param thisSetting
 * @param thisDefault
 * @return
 */
QString MyOrgSettings::readSettings(const QString &thisSetting, const QString &thisDefault)
{
    setMessage("read Settings", QString("readSettings(%1, %2)").arg(thisSetting, thisDefault), Debug);
    mySettings->beginGroup("General");
    QString theSetting = mySettings->value(thisSetting, thisDefault).toString();
    mySettings->endGroup();
    return theSetting;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::readSettings
 * @param thisSetting
 * @return
 */
QString MyOrgSettings::readSettings(const QString &thisSetting)
{
    setMessage("read Settings", QString("readSettings(%1)").arg(thisSetting), Debug);
    mySettings->beginGroup("General");
    QString theSetting = mySettings->value(thisSetting).toString();
    mySettings->endGroup();
    return theSetting;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::readSettingsBool
 * @param thisSetting
 * @param thisDefault
 * @return
 */
bool MyOrgSettings::readSettingsBool(const QString &thisSetting, bool thisDefault)
{
    setMessage("read Settings Bool", QString("readSettingsBool(%1, %2)").arg(thisSetting, thisDefault), Debug);
    mySettings->beginGroup("General");
    bool theSetting = mySettings->value(thisSetting, thisDefault).toBool();
    mySettings->endGroup();
    return theSetting;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::readSettingsInt
 * @param thisSetting
 * @param thisDefault
 * @return
 */
int MyOrgSettings::readSettingsInt(const QString &thisSetting, int thisDefault)
{
    setMessage("read Settings Int", QString("readSettingsInt(%1, %2)").arg(thisSetting, thisDefault), Debug);
    mySettings->beginGroup("General");
    int theSetting = mySettings->value(thisSetting, thisDefault).toInt();
    mySettings->endGroup();
    return theSetting;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::writeSettings
 * @param thisSetting
 * @param thisValue
 * @return
 */
bool MyOrgSettings::writeSettings(const QString &thisSetting, const QString &thisValue)
{
    setMessage("write Settings", QString("writeSettings(%1, %2)").arg(thisSetting, thisValue), Debug);
    mySettings->beginGroup("General");
    mySettings->setValue(thisSetting, thisValue);
    mySettings->endGroup();
    if (readSettings(thisSetting) == thisValue) { return true; }
    else { return false; }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getDataPath
 * @param thisFileName
 * @return
 */
QString MyOrgSettings::getDataPath(const QString &thisFileName)
{
    setMessage("get Data Path", QString("getDataPath(%1)").arg(thisFileName), Debug);
    //
    QDir settingsPath;
    // FIXME this is where the exe is
    QString myDataPath = QString("%1%2%3").arg(settingsPath.currentPath(), QDir::separator(), thisFileName);
    setMessage("", "getDataPath = " + myDataPath, Debug);
    return myDataPath;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getLastApplicationPath
 * @return
 */
QString MyOrgSettings::getLastApplicationPath()
{
    if (myLastApplicationFolder.isEmpty())
    {
        //
        QString theProjectPath = QCoreApplication::applicationFilePath();
        if (!isPathExists(theProjectPath))
        {
            setMessage("get Last Application Path", QString("%1 getLastApplicationPath(%2) - %3").arg(tr("getLastApplicationPath not found"), myLastApplicationFolder, theProjectPath), Warning);
            theProjectPath = QDir::currentPath();
        }
        setLastApplicationPath(readSettings(MY_LAST_PROJECT_FOLDER, theProjectPath));
    }
    setMessage("get Last Application Path", QString("getLastApplicationPath(%1)").arg(myLastApplicationFolder), Debug);
    return myLastApplicationFolder;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setLastApplicationPath
 * @param thisPath
 */
void MyOrgSettings::setLastApplicationPath(const QString &thisPath)
{
    setMessage("set Last Application Path", QString("setLastApplicationPath(%1)").arg(thisPath), Debug);
    myLastApplicationFolder = thisPath;
    writeSettings(MY_LAST_PROJECT_FOLDER, thisPath);
}
/*****************************************************************************/

/************************************************
 * @brief This value is used for Qt Settings: GitHub Account Name is one example.
 * getOrgName
 ***********************************************/
/**
 * @brief MyOrgSettings::getOrgName
 * @return
 */
QString MyOrgSettings::getOrgName()
{
    if (myOrganizationName.isEmpty()) { setOrgName(qApp->organizationName()); }
    setMessage("get Org Name", QString("getOrgName(%1)").arg(myOrganizationName), Debug);
    return myOrganizationName;
} // end getOrgName
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setOrgName
 * @param thisOrgName
 */
void MyOrgSettings::setOrgName(const QString &thisOrgName)
{
    setMessage("set Org Name", QString("setOrgName(%1)").arg(thisOrgName), Debug);
    if (QString::compare(myOrganizationName, thisOrgName, Qt::CaseInsensitive))
    {
        myOrganizationName = thisOrgName;
        emit sendUpdateSettings();
    }
} // end setOrgName
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getOrgDomain
 * @return
 */
QString MyOrgSettings::getOrgDomain()
{
    if (myOrganizationDomain.isEmpty()) { setOrgDomain(qApp->organizationDomain()); }
    setMessage("get Org Domain", QString("getOrgDomain(%1)").arg(myOrganizationDomain), Debug);
    return myOrganizationDomain;
} // end getOrgDomain
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setOrgDomain
 * @param thisOrgDomain
 */
void MyOrgSettings::setOrgDomain(const QString &thisOrgDomain)
{
    setMessage("set Org Domain", QString("setOrgDomain(%1)").arg(thisOrgDomain), Debug);
    if (QString::compare(myOrganizationDomain, thisOrgDomain, Qt::CaseInsensitive))
    {
        myOrganizationDomain = thisOrgDomain;
        emit sendUpdateSettings();
    }
} // end setOrgDomain
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getAppName
 * @return
 */
QString MyOrgSettings::getAppName()
{
    if (myApplicationName.isEmpty()) { setAppName(qApp->applicationName()); }
    setMessage("get App Name", QString("getAppName(%1)").arg(myApplicationName), Debug);
    return myApplicationName;
} // end getAppName
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setAppName
 * @param thisAppName
 */
void MyOrgSettings::setAppName(const QString &thisAppName)
{
    setMessage("set App Name", QString("setAppName(%1)").arg(thisAppName), Debug);
    if (QString::compare(myApplicationName, thisAppName, Qt::CaseInsensitive))
    {
        myApplicationName = thisAppName;
        emit sendUpdateSettings();
    }
} // end setAppName
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getVersion get Version is set in header as a default.
 * @return
 */
QString MyOrgSettings::getVersion()
{
    if (myVersion.isEmpty())
    {
        myVersion = qApp->applicationVersion();
    }
    setMessage("get Version", QString("getVersion(%1)").arg(myVersion), Debug);
    return myVersion;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setVersion
 * @param thisVersion
 * theLanguageModel->mySetting->setVersion(VERSION);
 */
void MyOrgSettings::setVersion(const QString &thisVersion)
{
    setMessage("set Version", QString("setVersion(%1)").arg(thisVersion), Debug);
    if (QString::compare(myVersion, thisVersion, Qt::CaseInsensitive))
    {
        myVersion = thisVersion;
        emit sendUpdateSettings();
    }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getCheckInternetUrl
 * @return
 */
QString MyOrgSettings::getCheckInternetUrl()
{
    setMessage("get CheckInternetUrl", QString("getCheckInternetUrl(%1)").arg(myCheckInternetUrl), Debug);
    if (myCheckInternetUrl.isEmpty())
        { setCheckInternetUrl(MY_INTERNET_URL); }
    return myCheckInternetUrl;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setCheckInternetUrl
 * @param thisCheckInternetUrl
 * theLanguageModel->mySetting->setVersion(VERSION);
 */
void MyOrgSettings::setCheckInternetUrl(const QString &thisCheckInternetUrl)
{
    setMessage("set CheckInternetUrl", QString("setCheckInternetUrl(%1)").arg(thisCheckInternetUrl), Debug);
    if (QString::compare(myCheckInternetUrl, thisCheckInternetUrl, Qt::CaseInsensitive))
    {
        myCheckInternetUrl = thisCheckInternetUrl;
        emit sendUpdateSettings();
    }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getIniFileName
 * @return
 */
QString MyOrgSettings::getIniFileName()
{
    if (myIniFileName.isEmpty())
        { setIniFileName(getAppName()); }
    setMessage("get Ini File", QString("getIniFile(%1)").arg(myIniFileName), Debug);
    return myIniFileName;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setIniFileName
 * @param thisIniFileName
 */
void MyOrgSettings::setIniFileName(const QString &thisIniFileName)
{
    setMessage("set Ini File", QString("setIniFile(%1)").arg(thisIniFileName), Debug);
    if (QString::compare(myIniFileName, thisIniFileName, Qt::CaseInsensitive))
    {
        myIniFileName = thisIniFileName;
        emit sendUpdateSettings();
    }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getIniFileExtension
 * @return
 */
QString MyOrgSettings::getIniFileExtension()
{
    setMessage("get Ini File", QString("getIniFile(%1)").arg(myIniFileName), Debug);
    if (myIniFileExtension.isEmpty())
        { setIniFileExtension(MY_INI_EXTENSION); }
    return myIniFileExtension;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setIniFileExtension
 * @param thisIniFileNameExtension
 */
void MyOrgSettings::setIniFileExtension(const QString &thisIniFileNameExtension)
{
    setMessage("set Ini File", QString("setIniFile(%1)").arg(thisIniFileNameExtension), Debug);
    if (QString::compare(myIniFileExtension, thisIniFileNameExtension, Qt::CaseInsensitive))
    {
        myIniFileExtension = thisIniFileNameExtension;
        emit sendUpdateSettings();
    }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getIniFullPath
 * @return
 */
QString MyOrgSettings::getIniFullPath()
{
    setMessage("get Ini Full Path", "getIniFullPath", Debug);
    if (myIniFullPath.isEmpty())
    {
        isOrgSettup = false;
        // organizationName, organizationDomain, applicationName and applicationName
        // are set in main.cpp, and passed into Constuctor, so they are set
        QCoreApplication::setOrganizationName(getOrgName());
        QCoreApplication::setOrganizationDomain(getOrgDomain());
        QCoreApplication::setApplicationName(getAppName());
        //QCoreApplication::setApplicationDisplayName(getAppName());
        // see *.pro file where it is: DEFINES *= APP_VERSION=$${VERSION}
        // cmake does not have this yet, this is a hack till I FIXME
        QCoreApplication::setApplicationVersion(getVersion());
        setIniFullPath(QString("%1%2%3.%4").arg(getAppDataLocation(), QDir::separator(), getIniFileName(), getIniFileExtension()));
        isOrgSettup = true;
    }
    return myIniFullPath;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setIniFullPath
 * @param thisIniFileName
 */
void MyOrgSettings::setIniFullPath(const QString &thisIniFileName)
{
    setMessage("set Ini Full Path", QString("%1(%2)").arg("setIniFullPath", thisIniFileName), Debug);
    if (QString::compare(myIniFullPath, thisIniFileName, Qt::CaseInsensitive))
    {
        myIniFullPath = thisIniFileName;
        emit sendUpdateSettings();
    }
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setGeometry
 * @param thisGeometry
 */
void MyOrgSettings::setGeometry(QByteArray thisGeometry)
{
    setMessage("set Geometry", "setGeometry", Debug);
    mySettings->beginGroup("Geometry");
    mySettings->setValue(MY_GEOMETRY, thisGeometry);
    mySettings->endGroup();
} // end setGeometry
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getGeometry
 * @return
 */
QByteArray MyOrgSettings::getGeometry()
{
    setMessage("get Geometry", "getGeometry", Debug);
    // Geometry
    mySettings->beginGroup("Geometry");
    myGeometry = mySettings->value(MY_GEOMETRY).toByteArray();
    mySettings->endGroup();
    return myGeometry;
} // end getGeometry
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setWindowState
 * @param thisWindowState
 */
void MyOrgSettings::setWindowState(QByteArray thisWindowState)
{
    setMessage("set Window State", "setWindowState", Debug);
    mySettings->beginGroup("Geometry");
    mySettings->setValue(MY_WINDOW_STATES, thisWindowState);
    mySettings->endGroup();
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getWindowState
 * @return
 */
QByteArray MyOrgSettings::getWindowState()
{
    setMessage("get Window State", "getWindowState", Debug);
    // Window State
    mySettings->beginGroup("Geometry");
    myWindowState = mySettings->value(MY_WINDOW_STATES).toByteArray();
    mySettings->endGroup();
    return myWindowState;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::readFile
 * @param thisFileName
 * @return
 */
QString MyOrgSettings::readFile(const QString &thisFileName)
{
    setMessage("read File", QString("readFile(%1)").arg(thisFileName), Debug);
    QFile file(thisFileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        setMessage(QObject::tr("Error opening file for reading").toLocal8Bit(), QString("%1: %2").arg(QObject::tr("Could not open file for reading"), thisFileName).toLocal8Bit(), Critical);
        return "";
    }
    QTextStream in(&file);
    QString thisText = in.readAll();
    file.close();
    return thisText;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::writeFile
 * @param thisFileName
 * @param thisContent
 * @return
 */
bool MyOrgSettings::writeFile(const QString &thisFileName, const QString &thisContent)
{
    setMessage("write File", QString("writeFile(%1)").arg(thisFileName), Debug);
    QFile theFile(thisFileName);
    // Trying to open in WriteOnly and Text mode and Truncate file if contents exists
    if(!theFile.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate))
    {
        setMessage(QObject::tr("Error opening file for writing").toLocal8Bit(), QString("%1: %2").arg(QObject::tr("Could not open file for writing"), thisFileName).toLocal8Bit(), Critical);
        return false;
    }
    // Write to file
    QTextStream theFileStream(&theFile);
    theFileStream << thisContent;
    theFile.flush();
    theFile.close();
    return true;
} // end writeFile
/*****************************************************************************/
/**
 * @brief MyOrgSettings::encryptThis
 * @param thisSecret
 * @return
 */
QString MyOrgSettings::encryptThis(const QString &thisSecret)
{
    setMessage("encrypt This", "encryptThis", Debug);
    return QString::fromLatin1(myCrypto->encode(thisSecret.toLocal8Bit(), myHashKey));
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::decryptThis
 * @param thisSecret
 * @return
 */
QString MyOrgSettings::decryptThis(const QString &thisSecret)
{
    setMessage("decrypt This", "decryptThis", Debug);
    return QString::fromLatin1(myCrypto->removePadding(myCrypto->decode(thisSecret.toLatin1(), myHashKey)));
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getFileInfo
 * @param thisInfo
 * @param thisFileFolder
 * @return
 */
QString MyOrgSettings::getFileInfo(MyOrgSettings::MyFileinfo thisInfo, const QString &thisFileFolder)
{
    setMessage("get File Info", QString("getFileInfo(%1)").arg(thisFileFolder), Debug);
    if (thisFileFolder.isEmpty()) { return ""; }
    //
    QFileInfo theFileInfo(thisFileFolder);
    //
    switch (thisInfo)
    {
        case AbsoluteFilePath:
            // The absolute path name consists of the full path and the file name.
            return theFileInfo.absoluteFilePath();
        case AbsolutePath:
            // Returns a file's path absolute path. This doesn't include the file name..
            return theFileInfo.absolutePath();
        case BaseName:
            // Returns the base name of the file without the path.
            return theFileInfo.baseName();
        case BirthTime:
            // Returns the date and time when the file was created / born.
            return theFileInfo.birthTime().toString();
        case CanonicalPath:
            // Returns the file's path canonical path (excluding the file name), i.e. an absolute path without symbolic links or redundant "." or ".." elements.
            return theFileInfo.canonicalPath();
        case CanonicalFilePath:
            // Returns the canonical path including the file name, i.e. an absolute path without symbolic links or redundant "." or ".." elements.
            return theFileInfo.canonicalFilePath();
        case CompleteBaseName:
            // Returns the complete base name of the file without the path.
            // "/tmp/archive.tar.gz" == "archive.tar"
            return theFileInfo.completeBaseName();
        case CompleteSuffix:
            // Returns the complete suffix (extension) of the file.
            // "/tmp/archive.tar.gz" == "tar.gz"
            return theFileInfo.completeSuffix();
        case Directory:
            // Returns the path of the object's parent directory as a QDir object.
            return theFileInfo.dir().currentPath();
        case FileName:
            // Returns the name of the file, excluding the path. getFileInfo(FileName, "")
            return theFileInfo.fileName();
        case FilePath:
            // Returns the file name, including the path (which may be absolute or relative).
            return theFileInfo.filePath();
        case IsWritable:
            // Returns true if the user can write to the file; otherwise returns false.
            if (theFileInfo.isWritable()) return "true"; else return "false";
        case FileSize:
            return QString::number(theFileInfo.size());
        case IsFile:
            if (isFileExists(thisFileFolder)) return "true"; else return "false";
        case IsFolder:
            if (isPathExists(thisFileFolder)) return "true"; else return "false";
        case IsSymLink:
            if (theFileInfo.isSymLink()) return "true"; else return "false";
        case SymLinkTarget:
            if (theFileInfo.isSymLink()) return theFileInfo.symLinkTarget(); else return "";
    }
    return "";
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isAppLocationWriteable
 * @param thisFolder
 * @return
 */
bool MyOrgSettings::isAppLocationWriteable(const QString &thisFolder)
{
    setMessage("is App Data Location Good", QString("isAppDataLocationGood(%1)").arg(thisFolder), Debug);
    bool isGood = true;
    if (isFileMake(thisFolder, "ReadMe.txt", true))
    {
        if (getFileInfo(IsWritable, combinePathFileName(thisFolder, "ReadMe.txt")) == "false")
            { isGood = false; }
    }
    else
        { isGood = false; }
    if (getFileInfo(IsFolder, thisFolder) == "false")
        { isGood = false; }
    return isGood;
} // end isAppLocationWriteable
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getAppDataLocation
 * @return
 */
QString MyOrgSettings::getAppDataLocation()
{
    if (myAppDataLocation.isEmpty())
    {
        QString theAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (theAppDataLocation.isEmpty())
        {
            theAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
            if (theAppDataLocation.isEmpty())
            { theAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); }
            if (theAppDataLocation.isEmpty())
            { theAppDataLocation = QDir::currentPath(); }
        }
        if (!isAppLocationWriteable(theAppDataLocation))
        {
            theAppDataLocation = QDir::currentPath(); // FIXME
        }
        setAppDataLocation(theAppDataLocation);
    }
    setMessage("get App Data Location", QString("getAppDataLocation(%1)").arg(myAppDataLocation), Debug);
    return myAppDataLocation;
} // end getAppDataLocation
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setAppDataLocation
 * @param thisAppDataLocation
 */
void MyOrgSettings::setAppDataLocation(const QString &thisAppDataLocation)
{
    if (QString::compare(myAppDataLocation, thisAppDataLocation, Qt::CaseInsensitive))
    {
        myAppDataLocation = thisAppDataLocation;
        emit sendUpdateSettings();
    }
} // end setAppDataLocation
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getAppDocumentLocation
 * @return
 */
QString MyOrgSettings::getAppDocumentLocation()
{
    if (myAppDocumentLocation.isEmpty())
    {
        QString theAppDocumentLocation = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        if (theAppDocumentLocation.isEmpty())
        {
            theAppDocumentLocation = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
            if (theAppDocumentLocation.isEmpty())
                { theAppDocumentLocation = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation); }
            if (theAppDocumentLocation.isEmpty())
                { theAppDocumentLocation = QDir::currentPath(); }
        }
        if (!isAppLocationWriteable(theAppDocumentLocation))
            { theAppDocumentLocation = QDir::currentPath(); } // FIXME
        setAppDocumentLocation(theAppDocumentLocation);
    }
    setMessage("get App Document Location", QString("getAppDocumentLocation(%1)").arg(myAppDocumentLocation), Debug);
    return myAppDocumentLocation;
} // end getAppDocumentLocation
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setAppDocumentLocation
 * @param thisAppDocumentLocation
 */
void MyOrgSettings::setAppDocumentLocation(const QString &thisAppDocumentLocation)
{
    if (QString::compare(myAppDocumentLocation, thisAppDocumentLocation, Qt::CaseInsensitive))
    {
        myAppDocumentLocation = thisAppDocumentLocation;
        emit sendUpdateSettings();
    }
} // end setAppDocumentLocation
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isWord is Word only checks to see if any letters are in it.
 * @param thisString
 * @return
 */
bool MyOrgSettings::isWord(const QString &thisString) const
{
    setMessage("is Word", QString("isWord(%1)").arg(thisString), Debug);
    for(int i = 0; i < thisString.length(); i++)
    {
        if (thisString.at(i).isLetter())
            { return true; }
    }
    return false;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::delay
 * @param thisSeconds
 */
void MyOrgSettings::delay(int thisSeconds)
{
    setMessage("idelay", QString("delay(%1)").arg(thisSeconds), Debug);
    QEventLoop theDelayLoop;
    QTimer::singleShot(thisSeconds * 1000, &theDelayLoop, &QEventLoop::quit);
    theDelayLoop.exec();
} // end delay
/*****************************************************************************/

/************************************************
 * @brief ping Internet.
 * pingInternet
 * set in cmake
 ***********************************************/
//#define MY_INTERNET_CHECK_PING
#ifdef MY_INTERNET_CHECK_PING
/**
 * @brief MyOrgSettings::pingInternet
 * @return
 */
bool MyOrgSettings::pingInternet()
{
    setMessage("ping Internet", "pingInternet", Debug);
    QStringList parameters;
    #if defined(WIN32)
    parameters << "-n" << "1";
    #else
    parameters << "-c 1";
    #endif
    parameters << getCheckInternetUrl();
    int exitCode = QProcess::execute("ping", parameters);
    if (exitCode==0)
        { return true; }
    else
        { return false; }
} // end delay
#else
/*****************************************************************************/
/**
 * @brief MyOrgSettings::connectInternet
 * @return
 */
bool MyOrgSettings::connectInternet()
{
    setMessage("connect Internet", "connectInternet", Debug);
    bool isInternetAvailable = false;
    QNetworkAccessManager *theNetworkManager = new QNetworkAccessManager;
    QEventLoop theEventLoop;
    QObject::connect(theNetworkManager, &QNetworkAccessManager::finished, &theEventLoop, &QEventLoop::quit);
    QNetworkReply *reply = theNetworkManager->get(QNetworkRequest(QUrl(getCheckInternetUrl())));
    theEventLoop.exec();
    if (reply->bytesAvailable())
        { isInternetAvailable = true; }
    else
        { isInternetAvailable = false; }
    delete reply;
    delete theNetworkManager;
    return isInternetAvailable;
} // end delay
#endif
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getInternetWait
 * @return
 */
bool MyOrgSettings::getInternetWait()
{
    setMessage("get Internet Wait", "getInternetWait", Debug);
    #ifdef MY_INTERNET_CHECK_PING
    if (pingInternet()) { return true; }
    #else
    if (connectInternet()) { return true; }
    #endif

    bool isNotConnected = true;
    while (isNotConnected)
    {
        delay(30);
        #ifdef MY_INTERNET_CHECK_PING
        isNotConnected = pingInternet();
        #else
        isNotConnected = connectInternet();
        #endif
        emit sendInternetProgress();
    }
    return true;
} // end delay
/*****************************************************************************/
/**
 * @brief MyOrgSettings::fileNumberLines
 * @param thisFile
 * @return
 */
int MyOrgSettings::fileNumberLines(const QString &thisFile)
{
    setMessage("file Number Lines", QString("fileNumberLines(%1)").arg(thisFile), Debug);
    if (!isFileExists(thisFile)) { setMessage(QObject::tr("File not found").toLocal8Bit(), QString("%1: %2").arg(QObject::tr("File not found"), thisFile).toLocal8Bit(), Critical); return 0; }
    std::ifstream inFile(thisFile.toLocal8Bit());
    return std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
} // end fileNumberLines
/*****************************************************************************/
/**
 * @brief MyOrgSettings::removeAllFiles
 * @param thisFolder
 */
void MyOrgSettings::removeAllFiles(const QString &thisFolder)
{
    setMessage("remove All Files", QString("removeAllFiles(%1)").arg(thisFolder), Debug);
    if (!isPathExists(thisFolder)) { setMessage(QObject::tr("Folder not found").toLocal8Bit(), QString("%1: %2").arg(QObject::tr("Folder not found"), thisFolder).toLocal8Bit(), Critical); return;  }
    QDir theDirFiles(thisFolder);
    theDirFiles.setNameFilters(QStringList() << "*.*");
    theDirFiles.setFilter(QDir::Files);
    foreach(QString theDirFile, theDirFiles.entryList())
    {
        theDirFiles.remove(theDirFile);
    }
} // end removeAllFiles
/*****************************************************************************/
/**
 * @brief MyOrgSettings::combinePathFileName
 * @param thisPath
 * @param thisFileName
 * @return
 */
QString MyOrgSettings::combinePathFileName(const QString &thisPath,const  QString &thisFileName)
{
    setMessage("combine Path File Name", QString("combinePathFileName(%1, %2)").arg(thisPath, thisFileName), Debug);
    return QString("%1%2%3").arg(thisPath, QDir::separator(), thisFileName);
} // end combinePathFileName
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isPathExists
 * @param thisPath
 * @return
 */
bool MyOrgSettings::isPathExists(const QString &thisPath)
{
    setMessage("is Path Exists", QString("isPathExists(%1)").arg(thisPath), Debug);
    return QDir(thisPath).exists() && QFileInfo(thisPath).isDir();
} // end isPathExists
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isFileExists
 * @param thisFile
 * @return
 */
bool MyOrgSettings::isFileExists(const QString &thisFile)
{
    setMessage("is File Exists", QString("isFileExists(%1)").arg(thisFile), Debug);
    // check if file exists and if yes: Is it really a file and not directory?
    return QFileInfo::exists(thisFile) && QFileInfo(thisFile).isFile();
} // end isFileExists
/*****************************************************************************/
/**
 * @brief MyOrgSettings::removeFile
 * @param thisFile
 * @return
 */
bool MyOrgSettings::removeFile(const QString &thisFile)
{
    setMessage("remove File", QString("removeFile(%1)").arg(thisFile), Debug);
    if (isFileExists(thisFile))
    { return QFile::remove(thisFile); }
    return true;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isFileMake
 * @param thisPath
 * @param thisFileName
 * @param isText
 * @return
 */
bool MyOrgSettings::isFileMake(const QString &thisPath, const QString &thisFileName, bool isText)
{
    setMessage("is File Make", QString("isFileMake(%1, %2)").arg(thisPath, thisFileName), Debug);
    isCreated = false;
    QString thePath = combinePathFileName(thisPath, thisFileName);
    if (!isMakeDir(thisPath)) return false;
    if (!isFileExists(thePath))
    {
        QFile theFile(thePath);
        if (theFile.open(QIODevice::WriteOnly))
        {
            /* Point a QTextStream object at the file */
            QTextStream outStream(&theFile);
            /* Write the line to the file */
            if (isText)
            {
                outStream << QString("%1 %2").arg(QObject::tr("This file was created to test if this folder is writeable by"), QCoreApplication::applicationName()).toLocal8Bit();
            }
            /* Close the file */
            theFile.close();
            if (isFileExists(thePath)) { isCreated = true; }
            return isCreated;
        }
        else return false;
    }
    return true;
} // end isFileMake
/*****************************************************************************/
/**
 * @brief MyOrgSettings::isMakeDir
 * @param thisPath
 * @return
 */
bool MyOrgSettings::isMakeDir(const QString &thisPath)
{
    setMessage("is Make Dir", QString("isMakeDir(%1)").arg(thisPath), Debug);
    QDir dir(thisPath);
    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        { setMessage(QObject::tr("File System Error").toLocal8Bit(), QString("%1 %2").arg(tr("Error: cannot create file"), thisPath).toLocal8Bit(), Critical); }
        else return true;
    }
    return isPathExists(thisPath);
} // end isMakeDir
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setDebugMessage
 * @param thisState
 */
void MyOrgSettings::setDebugMessage(bool thisState)
{
    isDebugMessage = thisState;
    //myCrypto->setDebugMessage(thisState);
    setMessage("set Debug Message", QString("setDebugMessage(%1)").arg(thisState), Debug);
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::getDebugMessage
 * @return
 */
bool MyOrgSettings::getDebugMessage()
{
    setMessage("get Debug Message", QString("getDebugMessage(%1)").arg(isDebugMessage), Debug);
    return isDebugMessage;
}
/*****************************************************************************/
/**
 * @brief MyOrgSettings::questionYesNo Title and Question Yes No.
 * @param thisTitle
 * @param thisQuestion
 * @return bool
 */
bool MyOrgSettings::questionYesNo(const char *thisTitle, const char *thisQuestion) const
{
    if (setMessage(thisTitle, thisQuestion, MyMessageTypes::Question) == "true")
        { return true; }
    else
        { return false; }
} // end questionYesNo
/*****************************************************************************/
/**
 * @brief MyOrgSettings::showMessageBox information, question: 1 = true, 0 = false, warning, critical: theMessage.toLocal8Bit().
 * @param thisTitle
 * @param thisMessage
 * @param thisMessageType
 * @return
 */
QVariant MyOrgSettings::showMessageBox(const QString &thisTitle, const QString &thisMessage, MyMessageTypes thisMessageType) const
{
    return setMessage(thisTitle, thisMessage, thisMessageType);
} // end showMessageBox
/*****************************************************************************/
/**
 * @brief MyOrgSettings::setMessage
 * @param thisTitle Depending on mode, the Title of Dialog or Window
 * @param thisMessage The Message
 * @param thisMessageType Information, Question, Warning, Critical, Debug
 * @return
 */
QVariant MyOrgSettings::setMessage(const QString &thisTitle, const QString &thisMessage, MyMessageTypes thisMessageType) const
{
    switch (thisMessageType)
    {
        case Debug:
        {
        #ifdef MY_QML
            #ifdef DEBUG
            // FIXME
            if (isDebugMessage)
            {
                #ifdef QT_DEBUG
                    qDebug() << thisMessage;
                #else
                    std::cout << thisMessage.toStdString() << std::endl;
                #endif
            }
            #else
            if (isDebugMessage)
                { std::cout << thisTitle.toStdString() << " " << thisMessage.toStdString() << std::endl; }
            #endif
            return "";
        #else
            if (isDebugMessage)
            {
                #ifdef QT_DEBUG
                qDebug() << thisMessage;
                #else
                std::cout << thisMessage.toStdString() << std::endl;
                #endif
            }
            return "";
        #endif
        }
        case Information:
        {
        #ifdef MY_QML
            #ifdef QT_DEBUG
            // FIXME
            qInfo() << thisMessage;
            #else
            std::cout << thisMessage.toStdString() << std::endl;
            #endif
            return "";
        #else
            return QMessageBox::information(nullptr, thisTitle, thisMessage, QMessageBox::Ok);
        #endif
        }
        case Question:
        {
        #ifdef MY_QML
            // FIXME
            #ifdef QT_DEBUG
            qDebug() << thisMessage;
            #else
            std::cout << thisMessage.toStdString() << std::endl;
            #endif
            return "";
        #else
            QMessageBox theMsgBox;
            theMsgBox.setWindowTitle(QObject::tr(thisTitle.toLocal8Bit()));
            theMsgBox.setText(QObject::tr(thisMessage.toLocal8Bit()));
            theMsgBox.setStandardButtons(QMessageBox::Yes);
            theMsgBox.addButton(QMessageBox::No);
            theMsgBox.setDefaultButton(QMessageBox::No);
            if(theMsgBox.exec() == QMessageBox::Yes)
            {
                setMessage("", QObject::tr("Yes was clicked"), Debug);
                return true;
            }
            else
            {
                setMessage("", QObject::tr("Yes was not clicked"), Debug);
                return false;
            }

        #endif
        }
        case Warning:
        {
        #ifdef MY_QML
            #ifdef QT_DEBUG
            // FIXME
            qWarning() << thisMessage;
            #else
            std::cout << thisMessage.toStdString() << std::endl;
            #endif
            return "";
        #else
            return QMessageBox::warning(nullptr, thisTitle, thisMessage, QMessageBox::Ok);
        #endif
        }
        case Critical:
        {
        #ifdef MY_QML
            #ifdef QT_DEBUG
            // FIXME
            qCritical() << thisMessage;
            #else
            std::cout << thisMessage.toStdString() << std::endl;
            #endif
            return "";
        #else
            return QMessageBox::critical(nullptr, thisTitle, thisMessage, QMessageBox::Ok);
        #endif
        }
    } // end switch (thisMessageType)
    return QVariant();
} // end setMessage
/*** ************************* End of File ***********************************/
