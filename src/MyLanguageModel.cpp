/************************************************
* @file MyLanguageModel.cpp
* @brief My Abstact Model for QML GUI Translation,
* My Abstact Model is used to help with QML GUI Translation,
* I check variables before setting them to prevent triggers.
* @author Jeffrey Scott Flesher <Jeffrey.Scott.Flesher@>
 ***********************************************/
#include "MyLanguageModel.h"
/*****************************************************************************/
#ifdef MY_QML
    Q_GLOBAL_STATIC(MyLanguageModel, singletonInstance)
#endif
/*****************************************************************************/
/**
 * @brief MyLanguageModel::MyLanguageModel Constructor
 * @param parent
 */
MyLanguageModel::MyLanguageModel(QObject *parent) : QAbstractListModel(parent)
{
    // Orgization Settings for INI files and Local Storage
    mySetting = new MyOrgSettings(parent);
    //    theLanguageModel->setDebugMessage(isDebugMessage);
    setTransFilePrefix(qApp->organizationName()); //!< Prefix of Translation files
    setTranslationSource("translations");   //!< Relative Folder for Translation files
    setHelpSource("help");                  //!< Relative Folder for Help files
    setLanguages();
    // theLanguageModel->mySetting->setIniFileExtension("ini"); // default is ini
    mySetting->setVersion(qApp->applicationVersion());
    mySetting->setIniFileName(qApp->organizationName());
    // System Tray
    #ifdef MY_SYSTEM_TRAY
    myAppPath = mySetting->getAppDataLocation();
    // Create a context menu with two items
    QMenu *trayIconMenu = new QMenu();

    QAction * viewWindow = new QAction(tr("Open"), this);
    QAction * quitAction = new QAction(tr("Close"), this);

    /* to connect the signals clicks on menu items to the appropriate signals for QML. */
    connect(viewWindow, &QAction::triggered, this, &MyLanguageModel::signalShow);
    connect(quitAction, &QAction::triggered, this, &MyLanguageModel::signalQuit);

    trayIconMenu->addAction(viewWindow);
    trayIconMenu->addAction(quitAction);

    /* Initialize the tray icon, icon set, and specify the tooltip */
    myTrayIcon = new QSystemTrayIcon();
    myTrayIcon->setContextMenu(trayIconMenu);
    //myTrayIcon->setIcon(QIcon(":/images/logo-min.png"));
    myTrayIcon->show();
    myTrayIcon->setToolTip(tr("Light Wizard Tray Program Work with winimizing program to tray"));

    /* Also connect clicking on the icon to the signal handler of the pressing */
    connect(myTrayIcon, &QSystemTrayIcon::activated, this, &MyLanguageModel::iconActivated);
    #endif // MY_SYSTEM_TRAY
    getCpuTemperature();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::~MyLanguageModel Deconstructor.
 */
MyLanguageModel::~MyLanguageModel()
{
    myItems.clear();
    #ifndef MY_QML
    if (myTranslator != nullptr )
    {
        if (!myTranslator->isEmpty())
        {
            qApp->removeTranslator(myTranslator);
        }
    }
    #endif
}
/*****************************************************************************/
/**
 * @brief Window::getTemperature
 */
void MyLanguageModel::getCpuTemperature()
{
    #ifdef MY_SENSORS
    #ifndef Q_OS_WIN
    subfeature sub {"/sys/class/hwmon/hwmon0/temp1_input"};
    double thisReading = (sub.read() * 9/5) + 32;
    QImage image(36, 36, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    if (thisReading < 100)
    {
        painter.fillRect(image.rect(), Qt::green);
    }
    else if (thisReading > 100 && thisReading < 120)
    {
        painter.fillRect(image.rect(), Qt::yellow);
    }
    else
    {
        painter.fillRect(image.rect(), Qt::red);
    }
    painter.drawText(image.rect(), Qt::AlignCenter | Qt::AlignVCenter, QString::number(thisReading));
    if (!image.save(myAppPath + "/temperature.png"))
    {
        qDebug() << tr("Failed to make Temperature PNG");
    }
    #ifdef MY_SYSTEM_TRAY
        myTrayIcon->setIcon(QIcon(myAppPath + "/temperature.png"));
    #endif
    #endif
    // FIXME else
    #endif
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setTemperture
 * @param thisTemperature
 */
void MyLanguageModel::setTemperatureUnit(const QString &thisTemperature)
{
    if (thisTemperature != myTemperture)
    {
        myTemperture = thisTemperature;
        mySetting->writeSettings(mySetting->MY_TEMPERATURE_UNIT, thisTemperature);
        emit temperatureUnitChanged();
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::temperatureUnit (Imperial Metric Kelvin) FIXME gets called first.
 * @return
 */
QString MyLanguageModel::temperatureUnit()
{
    myTemperture = mySetting->readSettings(mySetting->MY_TEMPERATURE_UNIT, "Imperial"); //
    return myTemperture;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setSystemTrayEnabled Only get called a function
 * @param isEnabled
 */
void MyLanguageModel::setSystemTrayEnabled(bool isEnabled)
{
    if (isEnabled != isSystemTrayEnalbe)
    {
        isSystemTrayEnalbe = isEnabled;
        mySetting->writeSettings(mySetting->MY_SYSTEM_TRAY_ENABLED, QString::number(isEnabled)); // 0 or 1
        emit systemTrayEnabledChanged();
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::systemTrayEnabled this gets called first FIXME.
 * @return
 */
bool MyLanguageModel::systemTrayEnabled()
{
    isSystemTrayEnalbe = mySetting->readSettingsBool(mySetting->MY_SYSTEM_TRAY_ENABLED, true); //
    return isSystemTrayEnalbe;
}
/*****************************************************************************/
#ifdef MY_SYSTEM_TRAY
/**
 * @brief SystemTray::iconActivated The method that handles click on the application icon in the system tray
 * @param reason
 */
void MyLanguageModel::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            // In the case of pressing the signal on the icon tray in the call signal QML layer
            qDebug() << tr("Clicked on Icon");
            emit signalIconActivated();
            break;
        default:
            break;
    }
}
/*****************************************************************************/
/**
 * @brief SystemTray::hideIconTray
 */
void MyLanguageModel::hideIconTray()
{
    myTrayIcon->hide();
}
#endif
/*****************************************************************************/
#ifdef MY_QML
/**
 * @brief MyLanguageModel::qmlInstance
 * @return MyLanguageModel::instance()
 */
QObject *MyLanguageModel::qmlInstance(QQmlEngine *, QJSEngine *)
{
    static MyLanguageModel *theSingletonInstance = singletonInstance();
    QQmlEngine::setObjectOwnership(theSingletonInstance, QQmlEngine::CppOwnership);
    return theSingletonInstance;
}
#endif
/*****************************************************************************/
/**
 * @brief MyLanguageModel::addItem
 * @param thisName
 * @param thisNameTranslated
 */
void MyLanguageModel::addItem(const QString &thisName, const QString &thisNameTranslated)
{
    mySetting->setMessage("add Item", QString("addItem(%1, %2)").arg(thisName, thisNameTranslated), MyOrgSettings::MyMessageTypes::Debug);
    addItem(MyLanguageData(thisName, thisNameTranslated));
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::addItem
 * @param thisItem
 */
void MyLanguageModel::addItem(const MyLanguageData &thisItem)
{
    mySetting->setMessage("add Item", QString("addItem(%1, %2)").arg(thisItem.name(), thisItem.nameTranslated()), MyOrgSettings::MyMessageTypes::Debug);
    //beginInsertRows(QModelIndex(), rowCount(), rowCount());
    myItems << thisItem;
    //endInsertRows();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::removeItem
 * @param thisRow
 */
void MyLanguageModel::removeItem(int thisRow)
{
    mySetting->setMessage("remove Item", QString("removeItem(%1)").arg(thisRow), MyOrgSettings::MyMessageTypes::Debug);
    beginRemoveRows(QModelIndex(), thisRow, thisRow);
    myItems.removeAt(thisRow);
    removeRow(thisRow, QModelIndex());
    endRemoveRows();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::rowCount
 * @param parent
 * @return
 */
int MyLanguageModel::rowCount(const QModelIndex &parent) const
{
    mySetting->setMessage("row Count", "rowCount", MyOrgSettings::MyMessageTypes::Debug);
    Q_UNUSED(parent);
    return myItems.count();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::data
 * @param index
 * @param role
 * @return
 */
QVariant MyLanguageModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= myItems.count()) { return QVariant(); }
    mySetting->setMessage("data", QString("data(%1, %2)").arg(index.row(), role), MyOrgSettings::MyMessageTypes::Debug);
    const MyLanguageData &Item = myItems[index.row()];
    switch (role)
    {
    case Qt::DisplayRole:    { return QString("%1, %2").arg(index.column()).arg(index.row()); } // Numbers
    case NameRole:           { return Item.name(); }
    case NameTranslatedRole: { return tr(Item.nameTranslated().toLocal8Bit()); } // Translate this value
    default: break;
    }
    return QVariant();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::roleNames
 * @return
 */
QHash<int, QByteArray> MyLanguageModel::roleNames() const
{
    mySetting->setMessage("role Names", "roleNames", MyOrgSettings::MyMessageTypes::Debug);
    QHash<int, QByteArray> myRoles;
    myRoles[NameRole] = "name";
    myRoles[NameTranslatedRole] = "translation";
    return myRoles;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::readLanguage
 */
void MyLanguageModel::readLanguage()
{
    mySetting->setMessage("read Language", QString("readLanguage(%1)").arg(getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    setLanguageCode(mySetting->readSettings(mySetting->MY_LOCALE_LANG_CODE, getDefaultLanguageCode()));
    setLanguageName(mySetting->readSettings(mySetting->MY_LOCALE_LANG_NAME, getDefaultLanguageName()));
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::writeLanguage
 */
void MyLanguageModel::writeLanguage()
{
    mySetting->setMessage("write Language", QString("writeLanguage(%1, %2)").arg(getLanguageCode(), getLanguageName()), MyOrgSettings::MyMessageTypes::Debug);
    mySetting->writeSettings(mySetting->MY_LOCALE_LANG_CODE, getLanguageCode());
    mySetting->writeSettings(mySetting->MY_LOCALE_LANG_NAME, getLanguageName());
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateThis
 * @param thisPhrase
 * @return
 */
QString MyLanguageModel::translateThis(const QString &thisPhrase)
{
    return tr(thisPhrase.toLocal8Bit());
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setDebugMessage
 * @param thisState
 */
void MyLanguageModel::setDebugMessage(bool thisState)
{
    isDebugMessage = thisState;
    mySetting->setDebugMessage(thisState);
    //mySetting->myCrypto->setDebugMessage(thisState);
    // Debugging
    mySetting->writeSettings(mySetting->MY_IS_DEBUG_MESSAGE, isDebugMessage ? "true" : "false");
    //isDebugMessage = mySetting->readSettingsBool(mySetting->MY_IS_DEBUG_MESSAGE, isDebugMessage);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getDebugMessage
 * @return
 */
bool MyLanguageModel::getDebugMessage()
{
    return isDebugMessage;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setMainLoaded
 * @param thisMainLoaded
 */
void MyLanguageModel::setMainLoaded(bool thisMainLoaded)
{
    mySetting->setMessage("set Main Loaded", "setMainLoaded", MyOrgSettings::MyMessageTypes::Debug);
    isMainLoaded = thisMainLoaded;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getMainLoaded
 * @return
 */
bool MyLanguageModel::getMainLoaded()
{
    mySetting->setMessage("get Main Loaded", "getMainLoaded", MyOrgSettings::MyMessageTypes::Debug);
    return isMainLoaded;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguageList
 * @param thisLanguageList
 */
void MyLanguageModel::setLanguageList(const QStringList &thisLanguageList)
{
    if (myLanguages != thisLanguageList)
    {
        myLanguages = thisLanguageList;
        emit languageListChanged();
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageList
 * @return
 */
const QStringList &MyLanguageModel::getLanguageList() const
{
    //if (myLanguages.empty()) { setLanguages(); }
    return myLanguages;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::encryptThis
 * @param thisSecret
 * @return myLastCrypted
 */
QString MyLanguageModel::encryptThis(const QString &thisSecret)
{
    myLastCrypted = mySetting->encryptThis(thisSecret);
    return myLastCrypted;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::decryptThis
 * @param thisSecret
 * @return myLastCrypted
 */
QString MyLanguageModel::decryptThis(const QString &thisSecret)
{
    if (thisSecret.isEmpty()) { emit encryptChanged(); return ""; }
    myLastCrypted = mySetting->decryptThis(thisSecret);
    if (myLastCrypted.isEmpty())
    {
        mySetting->setMessage("decryptThis Failed: Secret was not Empty but reply was.", "decryptThis Failed", MyOrgSettings::MyMessageTypes::Critical);
        if (!mySetting->cryptoTest())
        {
            mySetting->setMessage("cryptoTest Failed: Secret was not Empty but reply was.", "cryptoTest Failed", MyOrgSettings::MyMessageTypes::Critical);
        }
    }
    emit encryptChanged();
    return myLastCrypted;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::readSettings
 * @param thisSetting
 * @param thisDefault
 * @return mySetting->readSettings(thisSetting, thisDefault)
 */
QString MyLanguageModel::readSettings(const QString &thisSetting, const QString &thisDefault)
{
    return mySetting->readSettings(thisSetting, thisDefault);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::writeSettings
 * @param thisSetting
 * @param thisValue
 * @return mySetting->writeSettings(thisSetting, thisValue)
 */
bool MyLanguageModel::writeSettings(const QString &thisSetting, const QString &thisValue)
{
    emit settingsChanged();
    return mySetting->writeSettings(thisSetting, thisValue);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguageCodeList
 * @param thisLanguageCodeList
 */
void MyLanguageModel::setLanguageCodeList(const QStringList &thisLanguageCodeList)
{
    if (myLanguageCodes != thisLanguageCodeList)
    {
        myLanguageCodes = thisLanguageCodeList;
        emit languageCodeListChanged();
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageCodeList
 * @return
 */
const QStringList &MyLanguageModel::getLanguageCodeList() const
{
    //if (myLanguageCodes.isEmpty()) { setLanguages(); }
    return myLanguageCodes;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguages
 */
void MyLanguageModel::setLanguages()
{
    mySetting->setMessage("set Languages", "setLanguages", MyOrgSettings::MyMessageTypes::Debug);
    const QStringList theQmFiles =  getQmFiles(getTranslationSource());
    for (int i = 0; i < theQmFiles.size(); ++i)
    {
        QString theLanguageName = getLanguageFromFile(getTransFilePrefix(), theQmFiles.at(i));
        addItem(MyLanguageData(theLanguageName, tr(theLanguageName.toLocal8Bit())));
        myLanguages.append(theLanguageName);
        myLanguageCodes.append(languageNameToCode(theLanguageName));
    } // end for
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguage
 * @param thisLanguageName
 * @return
 */
bool MyLanguageModel::setLanguage(const QString &thisLanguageName)
{
    mySetting->setMessage("set Language", QString("setLanguage(%1)").arg(thisLanguageName), MyOrgSettings::MyMessageTypes::Debug);
    if (thisLanguageName.isEmpty()) { return false; }
    // Get Language File
    QString theQmLanguageFile = getLanguageFile(languageNameToCode(thisLanguageName), getTranslationSource(), getTransFilePrefix());
    // Load Language
    if (myTranslator != nullptr ) { qApp->removeTranslator(myTranslator); }
    myTranslator = new QTranslator(qApp);
    if (myTranslator->load(theQmLanguageFile))
    {
        qApp->installTranslator(myTranslator);
        /* FIXME in MainWindow.h make sure to add a function to relates in
         * protected slots: virtual void changeEvent(QEvent * event) override;
         * see MainWindow.h and MainWindow.cpp
         */
        #ifdef MY_QML
        //myEngine->retranslate();
        if (QQmlEngine *engine = qmlEngine(this))
        {
           engine->retranslate();
        }
        #endif
        setLanguageName(thisLanguageName);
        setLanguageCode(languageNameToCode(thisLanguageName));
        writeLanguage();
        emit languageChanged(thisLanguageName);
    }
    else
    {
        mySetting->setMessage("set Language", QString("%1: %2").arg(tr("loadLanguage failed"), theQmLanguageFile), MyOrgSettings::MyMessageTypes::Critical);
        return false;
    }
    return true;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageFromFile
 * @param thisPrefix
 * @param thisQmLanguageFile
 * @return QString thisLangFile = getLanguageFromFile(getTransFilePrefix(), "?.qm");
 */
QString MyLanguageModel::getLanguageFromFile(const QString &thisPrefix, const QString &thisQmLanguageFile)
{
    mySetting->setMessage("get Language From File", QString("getLanguageFromFile(%1, %2)").arg(thisPrefix, thisQmLanguageFile), MyOrgSettings::MyMessageTypes::Debug);
    return getLocalizerCode(thisPrefix, thisQmLanguageFile);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLocalizerCode
 * @param thisPrefix
 * @param thisQmFile
 * @return
 */
QString MyLanguageModel::getLocalizerCode(const QString &thisPrefix, const QString &thisQmFile)
{
    mySetting->setMessage("get Localizer Code", QString("getLocalizerCode(%1, %2)").arg(thisPrefix, thisQmFile), MyOrgSettings::MyMessageTypes::Debug);
    QString theQmLang = getLocalizedCodeFromFile(thisPrefix, thisQmFile);
    if (theQmLang.contains("_"))
    { theQmLang = theQmLang.replace("_", "-"); }
    return languageCodeToName(theQmLang);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLocalizedCodeFromFile
 * @param thisPrefix
 * @param thisQmFile
 * @return
 */
QString MyLanguageModel::getLocalizedCodeFromFile(const QString &thisPrefix, const QString &thisQmFile)
{
    mySetting->setMessage("get Localized Code From File", QString("getLocalizedCodeFromFile(%1, %2)").arg(thisPrefix, thisQmFile), MyOrgSettings::MyMessageTypes::Debug);
    QFileInfo theFileInfo(thisQmFile);

    QString theFileName = theFileInfo.baseName();
    theFileName = theFileName.remove(QString("%1_").arg(thisPrefix));
    if (theFileName.contains("_"))
    {
        if (!theFileName.contains("zh"))
        { theFileName = theFileName.section("_", 0, 0); }
    }
    return theFileName;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getQmFiles
 * @param thisFolder
 * @return
 */
QStringList MyLanguageModel::getQmFiles(const QString &thisFolder)
{
    mySetting->setMessage("getQmFiles", QString("getQmFiles(%1)").arg(thisFolder), MyOrgSettings::MyMessageTypes::Debug);
    // Read from resource file
    QDir dir(QString(":/%1").arg(thisFolder));
    QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
    for (QString &fileName : fileNames)
    {
        fileName = dir.filePath(fileName);
        QFileInfo theFileInfo(fileName);
        QString theLanguage = theFileInfo.baseName();
        theLanguage = getLocalizerCode(myTransFilePrefix, theLanguage);
        myCurrentQmFiles.insert(theLanguage, fileName);
    }
    return fileNames;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getTsFiles
 * @param thisFolder
 * @return
 */
QStringList MyLanguageModel::getTsFiles(const QString &thisFolder)
{
    mySetting->setMessage("getTsFiles", QString("getTsFiles(%1)").arg(thisFolder), MyOrgSettings::MyMessageTypes::Debug);
    QDir dir(thisFolder);
    QStringList fileNames = dir.entryList(QStringList("*.ts"), QDir::Files, QDir::Name);
    for (QString &fileName : fileNames)
    { fileName = dir.filePath(fileName); }
    return fileNames;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLangCode
 * @param thisString
 * @return
 */
QString MyLanguageModel::getLangCode(const QString &thisString)
{
    mySetting->setMessage("getLangCode", QString("getLangCode(%1)").arg(thisString), MyOrgSettings::MyMessageTypes::Debug);
    QString theLangCode = thisString;
    if (theLangCode.indexOf('_') < 0) { return ""; }
    if (theLangCode.indexOf('.') > 0)
    { theLangCode = theLangCode.mid(0, theLangCode.indexOf('.')); }
    return theLangCode.mid(theLangCode.indexOf('_') + 1);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getTranslationSource
 * @return
 */
QString MyLanguageModel::getTranslationSource()
{
    mySetting->setMessage("get Translation Source", QString("getTranslationSource(%1)").arg(myTranslationSource), MyOrgSettings::MyMessageTypes::Debug);
    return myTranslationSource;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setTranslationSource
 * @param thisTranslationSource
 */
void MyLanguageModel::setTranslationSource(const QString &thisTranslationSource)
{
    mySetting->setMessage("set Translation Source", QString("setTranslationSource(%1)").arg(thisTranslationSource), MyOrgSettings::MyMessageTypes::Debug);
    myTranslationSource = thisTranslationSource;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getHelpSource
 * @return
 */
QString MyLanguageModel::getHelpSource()
{
    mySetting->setMessage("get Help Source", QString("getHelpSource(%1)").arg(myHelpSource), MyOrgSettings::MyMessageTypes::Debug);
    return myHelpSource;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setHelpSource
 * @param thisHelpSource
 */
void MyLanguageModel::setHelpSource(const QString &thisHelpSource)
{
    mySetting->setMessage("set Help Source", QString("setHelpSource(%1)").arg(thisHelpSource), MyOrgSettings::MyMessageTypes::Debug);
    myHelpSource = thisHelpSource;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getTransFilePrefix
 * @return
 */
QString MyLanguageModel::getTransFilePrefix()
{
    mySetting->setMessage("get Trans File Prefix", QString("getTransFilePrefix(%1)").arg(myTransFilePrefix), MyOrgSettings::MyMessageTypes::Debug);
    return myTransFilePrefix;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setTransFilePrefix
 * @param thisTransFilePrefix
 */
void MyLanguageModel::setTransFilePrefix(const QString &thisTransFilePrefix)
{
    mySetting->setMessage("set Trans File Prefix", QString("setTransFilePrefix(%1)").arg(thisTransFilePrefix), MyOrgSettings::MyMessageTypes::Debug);
    myTransFilePrefix = thisTransFilePrefix;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageFile
 * @param thisLanguageCode
 * @param thisPath
 * @param thisPrefix
 * @return
 */
QString MyLanguageModel::getLanguageFile(const QString &thisLanguageCode, const QString &thisPath, const QString &thisPrefix)
{
    mySetting->setMessage("get Language File", QString("getLanguageFile(%1, %2, %3)").arg(thisLanguageCode, thisPath, thisPrefix), MyOrgSettings::MyMessageTypes::Debug);
    QString theLang = thisLanguageCode;
    if (theLang.contains("-"))
    { theLang = theLang.replace("-", "_"); }
    const QStringList theQmFiles = getQmFiles(thisPath);
    // FIXME setLanguages() read from internal db myCurrentQmFiles
    for (int i = 0; i < theQmFiles.size(); ++i)
    {
        if (languageMatch(thisPrefix, theLang, theQmFiles.at(i)))
        { return theQmFiles.at(i); }
    }
    return "";
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::languageMatch
 * @param thisPrefix
 * @param thisLang
 * @param thisQmFile
 * @return
 */
bool MyLanguageModel::languageMatch(const QString &thisPrefix, const QString &thisLang, const QString &thisQmFile)
{
    mySetting->setMessage("language Match", QString("languageMatch(%1, %2, %3)").arg(thisPrefix, thisLang, thisQmFile), MyOrgSettings::MyMessageTypes::Debug);
    // qmFile: ProjectName_xx.qm
    QString theLang = thisLang;
    QString theLocalizer = getLocalizedCodeFromFile(thisPrefix, thisQmFile);
    if (theLocalizer.contains("_"))
    { theLocalizer = theLocalizer.section("_", 0, 0); }
    if (thisLang.contains("_"))
    { theLang = thisLang.section("_", 0, 0); }
    return theLocalizer == theLang;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getDefaultLanguageCode
 * @return
 */
QString MyLanguageModel::getDefaultLanguageCode()
{
    mySetting->setMessage("get Default LanguageCode", QString("getDefaultLanguageCode(%1)").arg(language(QLocale())), MyOrgSettings::MyMessageTypes::Debug);
    return language(QLocale());
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getDefaultLanguageName
 * @return
 */
QString MyLanguageModel::getDefaultLanguageName()
{
    mySetting->setMessage("get Default Lanuage Name", QString("getDefaultLanuageName=%1").arg(getLanguageName()), MyOrgSettings::MyMessageTypes::Debug);
    return getLanguageName();
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguageCode
 * @param thisLanguageCode
 */
void MyLanguageModel::setLanguageCode(const QString &thisLanguageCode)
{
    mySetting->setMessage("set Language Code", QString("setLanguageCode(%1)").arg(thisLanguageCode), MyOrgSettings::MyMessageTypes::Debug);
    if (myLanguageCode != thisLanguageCode)
    {
        myLanguageCode = thisLanguageCode;
        // FIXME
        //mySetting->setLanguageCode(thisLanguageCode);
        emit languageCodeChanged(thisLanguageCode);
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageCode
 * @return
 */
QString MyLanguageModel::getLanguageCode()
{
    if (myLanguageCode.isEmpty()) { setLanguageCode(getDefaultLanguageCode()); }
    mySetting->setMessage("get Language Code", QString("getLanguageCode(%1)").arg(myLanguageCode), MyOrgSettings::MyMessageTypes::Debug);
    return myLanguageCode;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setLanguageName
 * @param thisLanguageName
 */
void MyLanguageModel::setLanguageName(const QString &thisLanguageName)
{
    mySetting->setMessage("set Language Name", QString("setLanguageName(%1)").arg(thisLanguageName), MyOrgSettings::MyMessageTypes::Debug);
    myLanguageName = thisLanguageName;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getLanguageName
 * @return
 */
QString MyLanguageModel::getLanguageName()
{
    if (myLanguageName.isEmpty())
    {
        setLanguageName(languageCodeToName(getDefaultLanguageCode()));
    }
    mySetting->setMessage("get Language Name", QString("getLanguageName(%1)").arg(myLanguageName), MyOrgSettings::MyMessageTypes::Debug);
    return myLanguageName;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setSavedLanguageName
 * @param thisLanguageName
 */
void MyLanguageModel::setSavedLanguageName(const QString &thisLanguageName)
{
    if (thisLanguageName != getSavedLanguageName())
    {
        if (mySetting->writeSettings(mySetting->MY_LOCALE_LANG_NAME, thisLanguageName))
        {
            emit savedLanguageNameChanged(thisLanguageName);
        }
    }
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getSavedLanguageName
 * @return
 */
QString MyLanguageModel::getSavedLanguageName()
{
    return mySetting->readSettings(mySetting->MY_LOCALE_LANG_NAME, getLanguageName());
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::languageCodeToName
 * @param thisLangageCode
 * @return
 */
QString MyLanguageModel::languageCodeToName(const QString &thisLangageCode)
{
    mySetting->setMessage("language Code To Name", QString("languageCodeToName(%1)").arg(staticGenericLanguageCodeToName.value(thisLangageCode)), MyOrgSettings::MyMessageTypes::Debug);
    return staticGenericLanguageCodeToName.value(thisLangageCode);
} // end languageCodeToName
/*****************************************************************************/
/**
 * @brief MyLanguageModel::languageNameToCode
 * @param theLanguageName
 * @return
 */
QString MyLanguageModel::languageNameToCode(const QString &theLanguageName)
{
    mySetting->setMessage("language Name To Code", QString("languageNameToCode(%1)").arg(staticGenericLanguageNameToCode.value(theLanguageName)), MyOrgSettings::MyMessageTypes::Debug);
    return staticGenericLanguageNameToCode.value(theLanguageName);
} // end languageNameToCode
/******************************************************************************
 * @brief s_genericLanguageCodesName
 * s_genericLanguageCodesName
 *****************************************************************************/
const QMap<QString, QString> MyLanguageModel::staticGenericLanguageCodeToName =
    {
        { QStringLiteral("auto"),       QStringLiteral("Auto")                  },
        { QStringLiteral("af"),         QStringLiteral("Afrikaans")             },
        { QStringLiteral("sq"),         QStringLiteral("Albanian")              },
        { QStringLiteral("am"),         QStringLiteral("Amharic")               },
        { QStringLiteral("ar"),         QStringLiteral("Arabic")                },
        { QStringLiteral("hy"),         QStringLiteral("Armenian")              },
        { QStringLiteral("az"),         QStringLiteral("Azerbaijani")           },
        { QStringLiteral("ba"),         QStringLiteral("Bashkir")               },
        { QStringLiteral("eu"),         QStringLiteral("Basque")                },
        { QStringLiteral("be"),         QStringLiteral("Belarusian")            },
        { QStringLiteral("bn"),         QStringLiteral("Bengali")               },
        { QStringLiteral("bs"),         QStringLiteral("Bosnian")               },
        { QStringLiteral("bg"),         QStringLiteral("Bulgarian")             },
        { QStringLiteral("yue"),        QStringLiteral("Cantonese")             },
        { QStringLiteral("ca"),         QStringLiteral("Catalan")               },
        { QStringLiteral("ceb"),        QStringLiteral("Cebuano")               },
        { QStringLiteral("ny"),         QStringLiteral("Chichewa")              },
        { QStringLiteral("co"),         QStringLiteral("Corsican")              },
        { QStringLiteral("hr"),         QStringLiteral("Croatian")              },
        { QStringLiteral("cs"),         QStringLiteral("Czech")                 },
        { QStringLiteral("da"),         QStringLiteral("Danish")                },
        { QStringLiteral("nl"),         QStringLiteral("Dutch")                 },
        { QStringLiteral("en"),         QStringLiteral("English")               },
        { QStringLiteral("eo"),         QStringLiteral("Esperanto")             },
        { QStringLiteral("et"),         QStringLiteral("Estonian")              },
        { QStringLiteral("fj"),         QStringLiteral("Fijian")                },
        { QStringLiteral("fil"),        QStringLiteral("Filipino")              },
        { QStringLiteral("fi"),         QStringLiteral("Finnish")               },
        { QStringLiteral("fr"),         QStringLiteral("French")                },
        { QStringLiteral("fy"),         QStringLiteral("Frisian")               },
        { QStringLiteral("gl"),         QStringLiteral("Galician")              },
        { QStringLiteral("ka"),         QStringLiteral("Georgian")              },
        { QStringLiteral("de"),         QStringLiteral("German")                },
        { QStringLiteral("el"),         QStringLiteral("Greek")                 },
        { QStringLiteral("gu"),         QStringLiteral("Gujarati")              },
        { QStringLiteral("ht"),         QStringLiteral("HaitianCreole")         },
        { QStringLiteral("ha"),         QStringLiteral("Hausa")                 },
        { QStringLiteral("haw"),        QStringLiteral("Hawaiian")              },
        { QStringLiteral("he"),         QStringLiteral("Hebrew")                },
        { QStringLiteral("mrj"),        QStringLiteral("HillMari")              },
        { QStringLiteral("hi"),         QStringLiteral("Hindi")                 },
        { QStringLiteral("hmn"),        QStringLiteral("Hmong")                 },
        { QStringLiteral("hu"),         QStringLiteral("Hungarian")             },
        { QStringLiteral("is"),         QStringLiteral("Icelandic")             },
        { QStringLiteral("ig"),         QStringLiteral("Igbo")                  },
        { QStringLiteral("id"),         QStringLiteral("Indonesian")            },
        { QStringLiteral("ga"),         QStringLiteral("Irish")                 },
        { QStringLiteral("it"),         QStringLiteral("Italian")               },
        { QStringLiteral("ja"),         QStringLiteral("Japanese")              },
        { QStringLiteral("jw"),         QStringLiteral("Javanese")              },
        { QStringLiteral("kn"),         QStringLiteral("Kannada")               },
        { QStringLiteral("kk"),         QStringLiteral("Kazakh")                },
        { QStringLiteral("km"),         QStringLiteral("Khmer")                 },
        { QStringLiteral("rw"),         QStringLiteral("Kinyarwanda")           },
        { QStringLiteral("tlh"),        QStringLiteral("Klingon")               },
        { QStringLiteral("tlh-Qaak"),   QStringLiteral("KlingonPlqaD")          },
        { QStringLiteral("ko"),         QStringLiteral("Korean")                },
        { QStringLiteral("ku"),         QStringLiteral("Kurdish")               },
        { QStringLiteral("ky"),         QStringLiteral("Kyrgyz")                },
        { QStringLiteral("lo"),         QStringLiteral("Lao")                   },
        { QStringLiteral("la"),         QStringLiteral("Latin")                 },
        { QStringLiteral("lv"),         QStringLiteral("Latvian")               },
        { QStringLiteral("apc"),        QStringLiteral("LevantineArabic")       },
        { QStringLiteral("lt"),         QStringLiteral("Lithuanian")            },
        { QStringLiteral("lb"),         QStringLiteral("Luxembourgish")         },
        { QStringLiteral("mk"),         QStringLiteral("Macedonian")            },
        { QStringLiteral("mg"),         QStringLiteral("Malagasy")              },
        { QStringLiteral("ms"),         QStringLiteral("Malay")                 },
        { QStringLiteral("ml"),         QStringLiteral("Malayalam")             },
        { QStringLiteral("mt"),         QStringLiteral("Maltese")               },
        { QStringLiteral("mi"),         QStringLiteral("Maori")                 },
        { QStringLiteral("mr"),         QStringLiteral("Marathi")               },
        { QStringLiteral("mhr"),        QStringLiteral("Mari")                  },
        { QStringLiteral("mn"),         QStringLiteral("Mongolian")             },
        { QStringLiteral("my"),         QStringLiteral("Myanmar")               },
        { QStringLiteral("ne"),         QStringLiteral("Nepali")                },
        { QStringLiteral("no"),         QStringLiteral("Norwegian")             },
        { QStringLiteral("or"),         QStringLiteral("Oriya")                 },
        { QStringLiteral("pap"),        QStringLiteral("Papiamento")            },
        { QStringLiteral("ps"),         QStringLiteral("Pashto")                },
        { QStringLiteral("fa"),         QStringLiteral("Persian")               },
        { QStringLiteral("pl"),         QStringLiteral("Polish")                },
        { QStringLiteral("pt"),         QStringLiteral("Portuguese")            },
        { QStringLiteral("pa"),         QStringLiteral("Punjabi")               },
        { QStringLiteral("otq"),        QStringLiteral("QueretaroOtomi")        },
        { QStringLiteral("ro"),         QStringLiteral("Romanian")              },
        { QStringLiteral("ru"),         QStringLiteral("Russian")               },
        { QStringLiteral("sm"),         QStringLiteral("Samoan")                },
        { QStringLiteral("gd"),         QStringLiteral("ScotsGaelic")           },
        { QStringLiteral("sr"),         QStringLiteral("SerbianCyrillic")       },
        { QStringLiteral("sr-Latin"),   QStringLiteral("SerbianLatin")          },
        { QStringLiteral("st"),         QStringLiteral("Sesotho")               },
        { QStringLiteral("sn"),         QStringLiteral("Shona")                 },
        { QStringLiteral("zh-CN"),      QStringLiteral("SimplifiedChinese")     },
        { QStringLiteral("sd"),         QStringLiteral("Sindhi")                },
        { QStringLiteral("si"),         QStringLiteral("Sinhala")               },
        { QStringLiteral("sk"),         QStringLiteral("Slovak")                },
        { QStringLiteral("sl"),         QStringLiteral("Slovenian")             },
        { QStringLiteral("so"),         QStringLiteral("Somali")                },
        { QStringLiteral("es"),         QStringLiteral("Spanish")               },
        { QStringLiteral("su"),         QStringLiteral("Sundanese")             },
        { QStringLiteral("sw"),         QStringLiteral("Swahili")               },
        { QStringLiteral("sv"),         QStringLiteral("Swedish")               },
        { QStringLiteral("tl"),         QStringLiteral("Tagalog")               },
        { QStringLiteral("ty"),         QStringLiteral("Tahitian")              },
        { QStringLiteral("tg"),         QStringLiteral("Tajik")                 },
        { QStringLiteral("ta"),         QStringLiteral("Tamil")                 },
        { QStringLiteral("tt"),         QStringLiteral("Tatar")                 },
        { QStringLiteral("te"),         QStringLiteral("Telugu")                },
        { QStringLiteral("th"),         QStringLiteral("Thai")                  },
        { QStringLiteral("to"),         QStringLiteral("Tongan")                },
        { QStringLiteral("zh-TW"),      QStringLiteral("TraditionalChinese")    },
        { QStringLiteral("tr"),         QStringLiteral("Turkish")               },
        { QStringLiteral("tk"),         QStringLiteral("Turkmen")               },
        { QStringLiteral("udm"),        QStringLiteral("Udmurt")                },
        { QStringLiteral("ug"),         QStringLiteral("Uighur")                },
        { QStringLiteral("uk"),         QStringLiteral("Ukrainian")             },
        { QStringLiteral("ur"),         QStringLiteral("Urdu")                  },
        { QStringLiteral("uz"),         QStringLiteral("Uzbek")                 },
        { QStringLiteral("vi"),         QStringLiteral("Vietnamese")            },
        { QStringLiteral("cy"),         QStringLiteral("Welsh")                 },
        { QStringLiteral("xh"),         QStringLiteral("Xhosa")                 },
        { QStringLiteral("yi"),         QStringLiteral("Yiddish")               },
        { QStringLiteral("yo"),         QStringLiteral("Yoruba")                },
        { QStringLiteral("yua"),        QStringLiteral("YucatecMaya")           },
        { QStringLiteral("zu"),         QStringLiteral("Zulu")                  }
}; // end s_genericLanguageCodeToName
/******************************************************************************
 * @brief language.
 * language
 * @param thisLocale QLocale given QLocale() returns Country Code
 *****************************************************************************/
QString MyLanguageModel::language(const QLocale &thisLocale)
{
    mySetting->setMessage("language", QString("language(%1)").arg(thisLocale.language()), MyOrgSettings::MyMessageTypes::Debug);
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
            else if (thisLocale.name() == "Chichewa")
            { return QStringLiteral("ny"); }
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
/******************************************************************************
 * @brief Added by Light-Wizzard s_genericLanguageCodesName
 * s_genericLanguageCodesName
 *****************************************************************************/
const QMap<QString, QString> MyLanguageModel::staticGenericLanguageNameToCode =
{
        { QStringLiteral("Auto"),                  QStringLiteral("auto")     },
        { QStringLiteral("Afrikaans"),             QStringLiteral("af")       },
        { QStringLiteral("Albanian"),              QStringLiteral("sq")       },
        { QStringLiteral("Amharic"),               QStringLiteral("am")       },
        { QStringLiteral("Arabic"),                QStringLiteral("ar")       },
        { QStringLiteral("Armenian"),              QStringLiteral("hy")       },
        { QStringLiteral("Azerbaijani"),           QStringLiteral("az")       },
        { QStringLiteral("Bashkir"),               QStringLiteral("ba")       },
        { QStringLiteral("Basque"),                QStringLiteral("eu")       },
        { QStringLiteral("Belarusian"),            QStringLiteral("be")       },
        { QStringLiteral("Bengali"),               QStringLiteral("bn")       },
        { QStringLiteral("Bosnian"),               QStringLiteral("bs")       },
        { QStringLiteral("Bulgarian"),             QStringLiteral("bg")       },
        { QStringLiteral("Cantonese"),             QStringLiteral("yue")      },
        { QStringLiteral("Catalan"),               QStringLiteral("ca")       },
        { QStringLiteral("Cebuano"),               QStringLiteral("ceb")      },
        { QStringLiteral("Chichewa"),              QStringLiteral("ny")       },
        { QStringLiteral("Corsican"),              QStringLiteral("co")       },
        { QStringLiteral("Croatian"),              QStringLiteral("hr")       },
        { QStringLiteral("Czech"),                 QStringLiteral("cs")       },
        { QStringLiteral("Danish"),                QStringLiteral("da")       },
        { QStringLiteral("Dutch"),                 QStringLiteral("nl")       },
        { QStringLiteral("English"),               QStringLiteral("en")       },
        { QStringLiteral("Esperanto"),             QStringLiteral("eo")       },
        { QStringLiteral("Estonian"),              QStringLiteral("et")       },
        { QStringLiteral("Fijian"),                QStringLiteral("fj")       },
        { QStringLiteral("Filipino"),              QStringLiteral("fil")      },
        { QStringLiteral("Finnish"),               QStringLiteral("fi")       },
        { QStringLiteral("French"),                QStringLiteral("fr")       },
        { QStringLiteral("Frisian"),               QStringLiteral("fy")       },
        { QStringLiteral("Galician"),              QStringLiteral("gl")       },
        { QStringLiteral("Georgian"),              QStringLiteral("ka")       },
        { QStringLiteral("German"),                QStringLiteral("de")       },
        { QStringLiteral("Greek"),                 QStringLiteral("el")       },
        { QStringLiteral("Gujarati"),              QStringLiteral("gu")       },
        { QStringLiteral("HaitianCreole"),         QStringLiteral("ht")       },
        { QStringLiteral("Hausa"),                 QStringLiteral("ha")       },
        { QStringLiteral("Hawaiian"),              QStringLiteral("haw")      },
        { QStringLiteral("Hebrew"),                QStringLiteral("he")       },
        { QStringLiteral("HillMari"),              QStringLiteral("mrj")      },
        { QStringLiteral("Hindi"),                 QStringLiteral("hi")       },
        { QStringLiteral("Hmong"),                 QStringLiteral("hmn")      },
        { QStringLiteral("Hungarian"),             QStringLiteral("hu")       },
        { QStringLiteral("Icelandic"),             QStringLiteral("is")       },
        { QStringLiteral("Igbo"),                  QStringLiteral("ig")       },
        { QStringLiteral("Indonesian"),            QStringLiteral("id")       },
        { QStringLiteral("Irish"),                 QStringLiteral("ga")       },
        { QStringLiteral("Italian"),               QStringLiteral("it")       },
        { QStringLiteral("Japanese"),              QStringLiteral("ja")       },
        { QStringLiteral("Javanese"),              QStringLiteral("jw")       },
        { QStringLiteral("Kannada"),               QStringLiteral("kn")       },
        { QStringLiteral("Kazakh"),                QStringLiteral("kk")       },
        { QStringLiteral("Khmer"),                 QStringLiteral("km")       },
        { QStringLiteral("Kinyarwanda"),           QStringLiteral("rw")       },
        { QStringLiteral("Klingon"),               QStringLiteral("tlh")      },
        { QStringLiteral("KlingonPlqaD"),          QStringLiteral("tlh-Qaak") },
        { QStringLiteral("Korean"),                QStringLiteral("ko")       },
        { QStringLiteral("Kurdish"),               QStringLiteral("ku")       },
        { QStringLiteral("Kyrgyz"),                QStringLiteral("ky")       },
        { QStringLiteral("Lao"),                   QStringLiteral("lo")       },
        { QStringLiteral("Latin"),                 QStringLiteral("la")       },
        { QStringLiteral("Latvian"),               QStringLiteral("lv")       },
        { QStringLiteral("LevantineArabic"),       QStringLiteral("apc")      },
        { QStringLiteral("Lithuanian"),            QStringLiteral("lt")       },
        { QStringLiteral("Luxembourgish"),         QStringLiteral("lb")       },
        { QStringLiteral("Macedonian"),            QStringLiteral("mk")       },
        { QStringLiteral("Malagasy"),              QStringLiteral("mg")       },
        { QStringLiteral("Malay"),                 QStringLiteral("ms")       },
        { QStringLiteral("Malayalam"),             QStringLiteral("ml")       },
        { QStringLiteral("Maltese"),               QStringLiteral("mt")       },
        { QStringLiteral("Maori"),                 QStringLiteral("mi")       },
        { QStringLiteral("Marathi"),               QStringLiteral("mr")       },
        { QStringLiteral("Mari"),                  QStringLiteral("mhr")      },
        { QStringLiteral("Mongolian"),             QStringLiteral("mn")       },
        { QStringLiteral("Myanmar"),               QStringLiteral("my")       },
        { QStringLiteral("Nepali"),                QStringLiteral("ne")       },
        { QStringLiteral("Norwegian"),             QStringLiteral("no")       },
        { QStringLiteral("Oriya"),                 QStringLiteral("or")       },
        { QStringLiteral("Papiamento"),            QStringLiteral("pap")      },
        { QStringLiteral("Pashto"),                QStringLiteral("ps")       },
        { QStringLiteral("Persian"),               QStringLiteral("fa")       },
        { QStringLiteral("Polish"),                QStringLiteral("pl")       },
        { QStringLiteral("Portuguese"),            QStringLiteral("pt")       },
        { QStringLiteral("Punjabi"),               QStringLiteral("pa")       },
        { QStringLiteral("QueretaroOtomi"),        QStringLiteral("otq")      },
        { QStringLiteral("Romanian"),              QStringLiteral("ro")       },
        { QStringLiteral("Russian"),               QStringLiteral("ru")       },
        { QStringLiteral("Samoan"),                QStringLiteral("sm")       },
        { QStringLiteral("ScotsGaelic"),           QStringLiteral("gd")       },
        { QStringLiteral("SerbianCyrillic"),       QStringLiteral("sr")       },
        { QStringLiteral("SerbianLatin"),          QStringLiteral("sr-Latin") },
        { QStringLiteral("Sesotho"),               QStringLiteral("st")       },
        { QStringLiteral("Shona"),                 QStringLiteral("sn")       },
        { QStringLiteral("SimplifiedChinese"),     QStringLiteral("zh-CN")    },
        { QStringLiteral("Sindhi"),                QStringLiteral("sd")       },
        { QStringLiteral("Sinhala"),               QStringLiteral("si")       },
        { QStringLiteral("Slovak"),                QStringLiteral("sk")       },
        { QStringLiteral("Slovenian"),             QStringLiteral("sl")       },
        { QStringLiteral("Somali"),                QStringLiteral("so")       },
        { QStringLiteral("Spanish"),               QStringLiteral("es")       },
        { QStringLiteral("Sundanese"),             QStringLiteral("su")       },
        { QStringLiteral("Swahili"),               QStringLiteral("sw")       },
        { QStringLiteral("Swedish"),               QStringLiteral("sv")       },
        { QStringLiteral("SerbianLatin"),          QStringLiteral("sr-Latin") },
        { QStringLiteral("Tagalog"),               QStringLiteral("tl")       },
        { QStringLiteral("Tahitian"),              QStringLiteral("ty")       },
        { QStringLiteral("Tajik"),                 QStringLiteral("tg")       },
        { QStringLiteral("Tamil"),                 QStringLiteral("ta")       },
        { QStringLiteral("Tatar"),                 QStringLiteral("tt")       },
        { QStringLiteral("Telugu"),                QStringLiteral("te")       },
        { QStringLiteral("Thai"),                  QStringLiteral("th")       },
        { QStringLiteral("Tongan"),                QStringLiteral("to")       },
        { QStringLiteral("TraditionalChinese"),    QStringLiteral("zh-TW")    },
        { QStringLiteral("Turkish"),               QStringLiteral("tr")       },
        { QStringLiteral("Turkmen"),               QStringLiteral("tk")       },
        { QStringLiteral("Udmurt"),                QStringLiteral("udm")      },
        { QStringLiteral("Uighur"),                QStringLiteral("ug")       },
        { QStringLiteral("Ukrainian"),             QStringLiteral("uk")       },
        { QStringLiteral("Urdu"),                  QStringLiteral("ur")       },
        { QStringLiteral("Uzbek"),                 QStringLiteral("uz")       },
        { QStringLiteral("Udmurt"),                QStringLiteral("udm")      },
        { QStringLiteral("Vietnamese"),            QStringLiteral("vi")       },
        { QStringLiteral("Welsh"),                 QStringLiteral("cy")       },
        { QStringLiteral("Xhosa"),                 QStringLiteral("xh")       },
        { QStringLiteral("Yiddish"),               QStringLiteral("yi")       },
        { QStringLiteral("Yoruba"),                QStringLiteral("yo")       },
        { QStringLiteral("YucatecMaya"),           QStringLiteral("yua")      },
        { QStringLiteral("YucatecMaya"),           QStringLiteral("yua")      },
        { QStringLiteral("Zulu"),                  QStringLiteral("zu")       }
}; // end s_genericLanguageNameToCode
/******************************************************************************
 * @brief Added by Light-Wizzard s_genericLanguageNameToDateTimeFormat
 * s_genericLanguageNameToDateTimeFormat
 *****************************************************************************/
const QMap<QString, QString> MyLanguageModel::staticGenericLanguageNameToDateTimeFormat =
{
        { QStringLiteral("Auto"),                  QStringLiteral("auto")     },
        { QStringLiteral("Afrikaans"),             QStringLiteral("yyyy/MM/dd")       },
        { QStringLiteral("Albanian"),              QStringLiteral("yyyy-MM-dd")       },
        { QStringLiteral("Amharic"),               QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("Arabic"),                QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Armenian"),              QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Azerbaijani"),           QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Bashkir"),               QStringLiteral("dd.MM.yy")       },
        { QStringLiteral("Basque"),                QStringLiteral("yyyy/MM/dd")       },
        { QStringLiteral("Belarusian"),            QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Bengali"),               QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("Bosnian"),               QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("Bulgarian"),             QStringLiteral("dd.M.yyyy")       },
        { QStringLiteral("Cantonese"),             QStringLiteral("yue")      },
        { QStringLiteral("Catalan"),               QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Cebuano"),               QStringLiteral("ceb")      },
        { QStringLiteral("Chichewa"),              QStringLiteral("ny")       },
        { QStringLiteral("Corsican"),              QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Croatian"),              QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("Czech"),                 QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("Danish"),                QStringLiteral("dd-MM-yyyy")       },
        { QStringLiteral("Dutch"),                 QStringLiteral("d-M-yyyy")       },
        { QStringLiteral("English"),               QStringLiteral("M/d/yyyy")       },
        { QStringLiteral("Esperanto"),             QStringLiteral("eo")       },
        { QStringLiteral("Estonian"),              QStringLiteral("et")       },
        { QStringLiteral("Fijian"),                QStringLiteral("fj")       },
        { QStringLiteral("Filipino"),              QStringLiteral("M/d/yyyy")      },
        { QStringLiteral("Finnish"),               QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("French"),                QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Frisian"),               QStringLiteral("d-M-yyyy")       },
        { QStringLiteral("Galician"),              QStringLiteral("dd/MM/yy")       },
        { QStringLiteral("Georgian"),              QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("German"),                QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Greek"),                 QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("Gujarati"),              QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("HaitianCreole"),         QStringLiteral("ht")       },
        { QStringLiteral("Hausa"),                 QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("Hawaiian"),              QStringLiteral("haw")      },
        { QStringLiteral("Hebrew"),                QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("HillMari"),              QStringLiteral("mrj")      },
        { QStringLiteral("Hindi"),                 QStringLiteral("dd-MM-yyyy")       },
        { QStringLiteral("Hmong"),                 QStringLiteral("hmn")      },
        { QStringLiteral("Hungarian"),             QStringLiteral("yyyy. MM. dd.")       },
        { QStringLiteral("Icelandic"),             QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("Igbo"),                  QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("Indonesian"),            QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Irish"),                 QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Italian"),               QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Japanese"),              QStringLiteral("yyyy/MM/dd")       },
        { QStringLiteral("Javanese"),              QStringLiteral("jw")       },
        { QStringLiteral("Kannada"),               QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("Kazakh"),                QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Khmer"),                 QStringLiteral("yyyy-MM-dd")       },
        { QStringLiteral("Kinyarwanda"),           QStringLiteral("M/d/yyyy")       },
        { QStringLiteral("Klingon"),               QStringLiteral("tlh")      },
        { QStringLiteral("KlingonPlqaD"),          QStringLiteral("tlh-Qaak") },
        { QStringLiteral("Korean"),                QStringLiteral("yyyy. MM. dd")       },
        { QStringLiteral("Kurdish"),               QStringLiteral("ku")       },
        { QStringLiteral("Kyrgyz"),                QStringLiteral("dd.MM.yy")       },
        { QStringLiteral("Lao"),                   QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Latin"),                 QStringLiteral("la")       },
        { QStringLiteral("Latvian"),               QStringLiteral("yyyy.MM.dd.")       },
        { QStringLiteral("LevantineArabic"),       QStringLiteral("apc")      },
        { QStringLiteral("Lithuanian"),            QStringLiteral("yyyy.MM.dd")       },
        { QStringLiteral("Luxembourgish"),         QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Macedonian"),            QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Malagasy"),              QStringLiteral("mg")       },
        { QStringLiteral("Malay"),                 QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Malayalam"),             QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("Maltese"),               QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Maori"),                 QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Marathi"),               QStringLiteral("dd-MM-yyyy")       },
        { QStringLiteral("Mari"),                  QStringLiteral("mhr")      },
        { QStringLiteral("Mongolian"),             QStringLiteral("yy.MM.dd")       },
        { QStringLiteral("Myanmar"),               QStringLiteral("my")       },
        { QStringLiteral("Nepali"),                QStringLiteral("M/d/yyyy")       },
        { QStringLiteral("Norwegian"),             QStringLiteral("no")       },
        { QStringLiteral("Oriya"),                 QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("Papiamento"),            QStringLiteral("pap")      },
        { QStringLiteral("Pashto"),                QStringLiteral("dd/MM/yy")       },
        { QStringLiteral("Persian"),               QStringLiteral("MM/dd/yyyy")       },
        { QStringLiteral("Polish"),                QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Portuguese"),            QStringLiteral("dd-MM-yyyy")       },
        { QStringLiteral("Punjabi"),               QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("QueretaroOtomi"),        QStringLiteral("otq")      },
        { QStringLiteral("Romanian"),              QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Russian"),               QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Samoan"),                QStringLiteral("sm")       },
        { QStringLiteral("ScotsGaelic"),           QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("SerbianCyrillic"),       QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("SerbianLatin"),          QStringLiteral("d.M.yyyy") },
        { QStringLiteral("Sesotho"),               QStringLiteral("st")       },
        { QStringLiteral("Shona"),                 QStringLiteral("sn")       },
        { QStringLiteral("SimplifiedChinese"),     QStringLiteral("yyyy/M/d")    },
        { QStringLiteral("Sindhi"),                QStringLiteral("sd")       },
        { QStringLiteral("Sinhala"),               QStringLiteral("yyyy-MM-dd")       },
        { QStringLiteral("Slovak"),                QStringLiteral("d. M. yyyy")       },
        { QStringLiteral("Slovenian"),             QStringLiteral("d.M.yyyy")       },
        { QStringLiteral("Somali"),                QStringLiteral("so")       },
        { QStringLiteral("Spanish"),               QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Sundanese"),             QStringLiteral("su")       },
        { QStringLiteral("Swahili"),               QStringLiteral("sw")       },
        { QStringLiteral("Swedish"),               QStringLiteral("yyyy-MM-dd")       },
        { QStringLiteral("Tagalog"),               QStringLiteral("tl")       },
        { QStringLiteral("Tahitian"),              QStringLiteral("ty")       },
        { QStringLiteral("Tajik"),                 QStringLiteral("dd.MM.yy")       },
        { QStringLiteral("Tamil"),                 QStringLiteral("dd-MM-yyyy")       },
        { QStringLiteral("Tatar"),                 QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Telugu"),                QStringLiteral("dd-MM-yy")       },
        { QStringLiteral("Thai"),                  QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("Tongan"),                QStringLiteral("to")       },
        { QStringLiteral("TraditionalChinese"),    QStringLiteral("yyyy/M/d")    },
        { QStringLiteral("Turkish"),               QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Turkmen"),               QStringLiteral("dd.MM.yy")       },
        { QStringLiteral("Udmurt"),                QStringLiteral("udm")      },
        { QStringLiteral("Uighur"),                QStringLiteral("yyyy-M-d")       },
        { QStringLiteral("Ukrainian"),             QStringLiteral("dd.MM.yyyy")       },
        { QStringLiteral("Urdu"),                  QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Uzbek"),                 QStringLiteral("dd/MM yyyy")       },
        { QStringLiteral("Udmurt"),                QStringLiteral("udm")      },
        { QStringLiteral("Vietnamese"),            QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Welsh"),                 QStringLiteral("dd/MM/yyyy")       },
        { QStringLiteral("Xhosa"),                 QStringLiteral("yyyy/MM/dd")       },
        { QStringLiteral("Yiddish"),               QStringLiteral("yi")       },
        { QStringLiteral("Yoruba"),                QStringLiteral("d/M/yyyy")       },
        { QStringLiteral("YucatecMaya"),           QStringLiteral("yua")      },
        { QStringLiteral("YucatecMaya"),           QStringLiteral("yua")      },
        { QStringLiteral("Zulu"),                  QStringLiteral("yyyy/MM/dd")       }
}; // end s_genericLanguageNameToDateTimeFormat
/******************************************************************************
 * @brief language Name.
 * languageName
 ******************************************************************************/
QString MyLanguageModel::languageName(const QLocale &thisLocale)
{
    mySetting->setMessage("languageName", QString("languageName(%1)").arg(thisLocale.language()), MyOrgSettings::MyMessageTypes::Debug);
    switch (thisLocale.language())
    {
    case QLocale::Afrikaans:             return tr("Afrikaans");
    case QLocale::Albanian:              return tr("Albanian");
    case QLocale::Amharic:               return tr("Amharic");
    case QLocale::Arabic:                return tr("Arabic");
    case QLocale::Armenian:              return tr("Armenian");
    case QLocale::Azerbaijani:           return tr("Azeerbaijani");
    case QLocale::Basque:                return tr("Basque");
    case QLocale::Bashkir:               return tr("Bashkir");
    case QLocale::Belarusian:            return tr("Belarusian");
    case QLocale::Bengali:               return tr("Bengali");
    case QLocale::Bosnian:               return tr("Bosnian");
    case QLocale::Bulgarian:             return tr("Bulgarian");
    case QLocale::Catalan:               return tr("Catalan");
    case QLocale::Cantonese:             return tr("Cantonese");
    case QLocale::Cebuano:               return tr("Cebuano");
    case QLocale::Chinese:               return tr("SimplifiedChinese");
    case QLocale::LiteraryChinese:       return tr("TraditionalChinese");
    case QLocale::Corsican:              return tr("Corsican");
    case QLocale::Croatian:              return tr("Croatian");
    case QLocale::Czech:                 return tr("Czech");
    case QLocale::Danish:                return tr("Danish");
    case QLocale::Dutch:                 return tr("Dutch");
    case QLocale::English:               return tr("English");
    case QLocale::Esperanto:             return tr("Esperanto");
    case QLocale::Estonian:              return tr("Estonian");
    case QLocale::Fijian:                return tr("Fijian");
    case QLocale::Filipino:              return tr("Filipino");
    case QLocale::Finnish:               return tr("Finnish");
    case QLocale::French:                return tr("French");
    case QLocale::Frisian:               return tr("Frisian");
    case QLocale::Galician:              return tr("Galician");
    case QLocale::Georgian:              return tr("Georgian");
    case QLocale::German:                return tr("German");
    case QLocale::Greek:                 return tr("Greek");
    case QLocale::Gujarati:              return tr("Gujarati");
    //case QLocale::HaitianCreole:       return tr("Haitian Creole");
    case QLocale::Hausa:                 return tr("Hausa");
    case QLocale::Hawaiian:              return tr("Hawaiian");
    case QLocale::Hebrew:                return tr("Hebrew");
    //case QLocale::HillMari:            return tr("Hill Mari");
    case QLocale::Hindi:                 return tr("Hindi");
    //case QLocale::Hmong:               return tr("Hmong");
    case QLocale::Hungarian:             return tr("Hungarian");
    case QLocale::Icelandic:             return tr("Icelandic");
    case QLocale::Igbo:                  return tr("Igbo");
    case QLocale::Indonesian:            return tr("Indonesian");
    case QLocale::Irish:                 return tr("Irish");
    case QLocale::Italian:               return tr("Italian");
    case QLocale::Japanese:              return tr("Japanese");
    case QLocale::Javanese:              return tr("Javanese");
    case QLocale::Kannada:               return tr("Kannada");
    case QLocale::Kazakh:                return tr("Kazakh");
    case QLocale::Khmer:                 return tr("Khmer");
    case QLocale::Kinyarwanda:           return tr("Kinyarwanda");
    //case QLocale::Klingon:             return tr("Klingon");
    //case QLocale::KlingonPlqaD:        return tr("Klingon (PlqaD)");
    case QLocale::Korean:                return tr("Korean");
    case QLocale::Kurdish:               return tr("Kurdish");
    //case QLocale::Kyrgyz:              return tr("Kyrgyz");
    case QLocale::Lao:                   return tr("Lao");
    case QLocale::Latin:                 return tr("Latin");
    case QLocale::Latvian:               return tr("Latvian");
    //case QLocale::LevantineArabic:     return tr("Levantine Arabic");
    case QLocale::Lithuanian:            return tr("Lithuanian");
    case QLocale::Luxembourgish:         return tr("Luxembourgish");
    case QLocale::Macedonian:            return tr("Macedonian");
    case QLocale::Malagasy:              return tr("Malagasy");
    case QLocale::Malay:                 return tr("Malay");
    case QLocale::Malayalam:             return tr("Malayalam");
    case QLocale::Maltese:               return tr("Maltese");
    case QLocale::Maori:                 return tr("Maori");
    case QLocale::Marathi:               return tr("Marathi");
    //case QLocale::Mari:                return tr("Mari");
    case QLocale::Mongolian:             return tr("Mongolian");
    //case QLocale::Myanmar:             return tr("Myanmar");
    case QLocale::Nepali:                return tr("Nepali");
    case QLocale::NorwegianBokmal:       return tr("Norwegian");
    case QLocale::Oriya:                 return tr("Oriya");
    //case QLocale::Chichewa:            return tr("Chichewa");
    case QLocale::Papiamento:            return tr("Papiamento");
    case QLocale::Pashto:                return tr("Pashto");
    case QLocale::Persian:               return tr("Persian");
    case QLocale::Polish:                return tr("Polish");
    case QLocale::Portuguese:            return tr("Portuguese");
    case QLocale::Punjabi:               return tr("Punjabi");
    //case QLocale::QueretaroOtomi:      return tr("Queretaro Otomi");
    case QLocale::Romanian:              return tr("Romanian");
    case QLocale::Russian:               return tr("Russian");
    case QLocale::Samoan:                return tr("Samoan");
    //case QLocale::ScotsGaelic:         return tr("Scots Gaelic");
    //case QLocale::SerbianCyrillic:     return tr("Serbian (Cyrillic)");
    //case QLocale::SerbianLatin:        return tr("Serbian (Latin)");
    //case QLocale::Sesotho:             return tr("Sesotho");
    case QLocale::Shona:                 return tr("Shona");
    case QLocale::Sindhi:                return tr("Sindhi");
    case QLocale::Sinhala:               return tr("Sinhala");
    case QLocale::Slovak:                return tr("Slovak");
    case QLocale::Slovenian:             return tr("Slovenian");
    case QLocale::Somali:                return tr("Somali");
    case QLocale::Spanish:               return tr("Spanish");
    case QLocale::Sundanese:             return tr("Sundanese");
    case QLocale::Swahili:               return tr("Swahili");
    case QLocale::Swedish:               return tr("Swedish");
    //case QLocale::Tagalog:             return tr("Tagalog");
    case QLocale::Tahitian:              return tr("Tahitian");
    case QLocale::Tajik:                 return tr("Tajik");
    case QLocale::Tamil:                 return tr("Tamil");
    case QLocale::Tatar:                 return tr("Tatar");
    case QLocale::Telugu:                return tr("Telugu");
    case QLocale::Thai:                  return tr("Thai");
    case QLocale::Tongan:                return tr("Tongan");
    case QLocale::Turkish:               return tr("Turkish");
    case QLocale::Turkmen:               return tr("Turkmen");
    //case QLocale::Udmurt:              return tr("Udmurt");
    case QLocale::Uighur:                return tr("Uighur");
    case QLocale::Ukrainian:             return tr("Ukrainian");
    case QLocale::Urdu:                  return tr("Urdu");
    case QLocale::Uzbek:                 return tr("Uzbek");
    case QLocale::Vietnamese:            return tr("Vietnamese");
    case QLocale::Welsh:                 return tr("Welsh");
    case QLocale::Xhosa:                 return tr("Xhosa");
    case QLocale::Yiddish:               return tr("Yiddish");
    case QLocale::Yoruba:                return tr("Yoruba");
    //case QLocale::YucatecMaya:         return tr("Yucatec Maya");
    case QLocale::Zulu:                  return tr("Zulu");
    default:
        if (thisLocale.name() == "HaitianCreole")
        { return tr("Haitian Creole"); }
        else if (thisLocale.name() == "HillMari")
        { return tr("Hill Mari"); }
        else if (thisLocale.name() == "Hmong")
        { return tr("Hmong"); }
        else if (thisLocale.name() == "Klingon")
        { return tr("Klingon"); }
        else if (thisLocale.name() == "KlingonPlqaD")
        { return tr("Klingon (PlqaD)"); }
        else if (thisLocale.name() == "Kyrgyz")
        { return tr("Kyrgyz"); }
        else if (thisLocale.name() == "LevantineArabic")
        { return tr("Levantine Arabic"); }
        else if (thisLocale.name() == "Mari")
        { return tr("Mari"); }
        else if (thisLocale.name() == "Myanmar")
        { return tr("Myanmar"); }
        else if (thisLocale.name() == "Chichewa")
        { return tr("Chichewa"); }
        else if (thisLocale.name() == "QueretaroOtomi")
        { return tr("Queretaro Otomi"); }
        else if (thisLocale.name() == "ScotsGaelic")
        { return tr("Scots Gaelic"); }
        else if (thisLocale.name() == "SerbianCyrillic")
        { return tr("Serbian (Cyrillic)"); }
        else if (thisLocale.name() == "SerbianLatin")
        { return tr("Serbian (Latin)"); }
        else if (thisLocale.name() == "Sesotho")
        { return tr("Sesotho"); }
        else if (thisLocale.name() == "Tagalog")
        { return tr("Tagalog"); }
        else if (thisLocale.name() == "Udmurt")
        { return tr("Udmurt"); }
        else if (thisLocale.name() == "YucatecMaya")
        { return tr("Yucatec Maya"); }
        return QString();
    } // end switch (lang)
} // end languageName(Language lang)
/*****************************************************************************/
/**
 * @brief locale Language.
 * localeLanguage
 * @param thisLocaleCode Locale Code returns Country Code
 */
QLocale MyLanguageModel::localeLanguage(const QString &thisLocaleCode)
{
    mySetting->setMessage("localeLanguage", QString("localeLanguage(%1)").arg(thisLocaleCode), MyOrgSettings::MyMessageTypes::Debug);
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
/*****************************************************************************/
/**
 * @brief MyLanguageModel::getDefaultDateFormat
 * @return
 */
QString MyLanguageModel::getDefaultDateFormat()
{
    return myDefaultDateFormat;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::setDefaultDateFormat
 * @param thisDefaultDateFormat
 */
void MyLanguageModel::setDefaultDateFormat(const QString &thisDefaultDateFormat)
{
    myDefaultDateFormat = thisDefaultDateFormat;
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateInt
 * @param thisInt
 * @return
 */
QString MyLanguageModel::translateInt(const QString &thisInt)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateInt", QString("%1 <- translateInt(%2) Code=%3").arg(QString::number(theLocale.toInt(thisInt)), thisInt, getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return QString::number(theLocale.toInt(thisInt));
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateDouble
 * @param thisDouble
 * @return
 */
QString MyLanguageModel::translateDouble(const QString &thisDouble)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateInt", QString("%1 <- translateInt(%2) Code=%3").arg(QString::number(theLocale.toDouble(thisDouble)), thisDouble, getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return QString::number(theLocale.toDouble(thisDouble));
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateShortMonth
 * @param thisMonth
 * @return
 */
QString MyLanguageModel::translateShortMonth(int thisMonth)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateShortMonth", QString("%1 <- translateShortMonth(%2) Code=%3").arg(theLocale.monthName(thisMonth, theLocale.ShortFormat), QString::number(thisMonth), getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return theLocale.monthName(thisMonth + 1, theLocale.ShortFormat);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateLongMonth
 * @param thisMonth
 * @return
 */
QString MyLanguageModel::translateLongMonth(int thisMonth)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateLongMonth", QString("%1 <- translateLongMonth(%2) Code=%3").arg(theLocale.monthName(thisMonth, theLocale.LongFormat), QString::number(thisMonth), getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return theLocale.monthName(thisMonth + 1, theLocale.LongFormat);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateShortWeek
 * @param thisWeek
 * @return
 */
QString MyLanguageModel::translateShortWeek(int thisWeek)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateShortWeek", QString("%1 <- translateShortWeek(%2) Code=%3").arg(theLocale.dayName(thisWeek, theLocale.ShortFormat), QString::number(thisWeek), getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return theLocale.dayName(thisWeek + 1, theLocale.ShortFormat);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateLongWeek
 * @param thisWeek
 * @return
 */
QString MyLanguageModel::translateLongWeek(int thisWeek)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    mySetting->setMessage("translateLongWeek", QString("%1 <- translateLongWeek(%2) Code=%3").arg(theLocale.dayName(thisWeek, theLocale.LongFormat), QString::number(thisWeek), getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return theLocale.dayName(thisWeek + 1, theLocale.LongFormat);
}
/*****************************************************************************/
/**
 * @brief MyLanguageModel::translateTime
 * @param thisTime
 * @param thisTimeFormat
 * @return
 */
QString MyLanguageModel::translateTime(const QString &thisTime, const QString &thisTimeFormat)
{
    QLocale theLocale(localeLanguage(getLanguageCode()));
    QTime theTimeAs = theLocale.toTime(thisTime, thisTimeFormat);
    QString theTime = theTimeAs.toString(thisTimeFormat);
    mySetting->setMessage("translateTime", QString("%1 <- translateTime(%2, %3) Code=%4").arg(theTime, thisTime, thisTimeFormat, getLanguageCode()), MyOrgSettings::MyMessageTypes::Debug);
    return theTime;
}
/******************************* End of File *********************************/
