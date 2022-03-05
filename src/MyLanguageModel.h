#ifndef MyLanguageMODEL_H
#define MyLanguageMODEL_H

#include <QCoreApplication>
//#include <QtDebug>
#include <QDebug>
#include <QStringList>
#include <QListIterator>
#include <QStandardItemModel>
#include <QAbstractListModel>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QTranslator>
#include <QVector>
//
#include <QtNetwork/QNetworkReply>
#include <QtQml/QQmlListProperty>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeopositioninfo.h>
//#include <qnetworkconfigmanager.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QLoggingCategory>
#include <QtPositioning/QGeoPositionInfoSource>
// System Tray
#ifdef MY_SYSTEM_TRAY
    #ifndef QT_NO_SYSTEMTRAYICON
        #include <QSystemTrayIcon>
    #endif
    #include <QAction>
    #include <QMenu>
#endif

#ifdef MY_SENSORS
    #include <qpainter.h>
    #ifndef Q_OS_WIN
        #include <sensors-c++/sensors.h>
        #include <sensors/sensors.h>
        using namespace sensors;
    // FIXME Widnows
    #endif
#endif

// Qml
#ifdef MY_QML
    #include <QQmlEngine>
    #include <QtQml>
#else
    #include <QAbstractItemView>
    #include <QAbstractItemView>
    #include <QHeaderView>
    #include <QTableView>
#endif
//
/*
 * This application uses http://openweathermap.org/api
 */
#define ZERO_KELVIN 273.15
//
#include "MyLanguageData.h"
#include "MyOrgSettings.h"
/************************************************
 * @brief Abstact Model used for ComboBox.
 * /class MyLanguageModel
 ***********************************************/
class MyLanguageModel : public QAbstractListModel
{
    Q_OBJECT
    #ifdef MY_QML
        // Language Code
        Q_PROPERTY(QString getLanguageCode READ getLanguageCode WRITE setLanguageCode NOTIFY languageCodeChanged)
        // Saved Language Name
        Q_PROPERTY(QString getSavedLanguageName READ getSavedLanguageName WRITE setSavedLanguageName NOTIFY savedLanguageNameChanged)
        // Language List
        Q_PROPERTY(QStringList getLanguageList READ getLanguageList WRITE setLanguageList NOTIFY languageListChanged)
        // Language List
        Q_PROPERTY(QStringList getLanguageCodeList READ getLanguageCodeList WRITE setLanguageCodeList NOTIFY languageCodeListChanged)
        // Temperture
        Q_PROPERTY(QString temperatureUnit READ temperatureUnit WRITE setTemperatureUnit NOTIFY temperatureUnitChanged)
        // System Tray Enabled
        Q_PROPERTY(bool systemTrayEnabled READ systemTrayEnabled WRITE setSystemTrayEnabled NOTIFY systemTrayEnabledChanged)
        // Disable Copy
        Q_DISABLE_COPY(MyLanguageModel)
    #endif
    // Public
    public:
        enum ItemRoles
        {
            NameRole = Qt::UserRole + 1, //!< Name Role
            NameTranslatedRole           //!< Name Translated Role
        };
        //
        MyLanguageModel(QObject *parent = nullptr); //!< My Abstact Model Constructor
        ~MyLanguageModel();  //!< Deconstructor
        //
        MyOrgSettings *mySetting;                       //!< \c mySetting            @brief my Setting
        // add Item
        void addItem(const QString &thisName, const QString &thisNameTranslated); //!< add Item
        void addItem(const MyLanguageData &thisItem);   //!< add Item
        // remove Item
        void removeItem(int thisRow);       //!< remove Item
        // row Count
        int rowCount(const QModelIndex & parent = QModelIndex()) const;             //!< row Count
        // data
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const; //!< data
        // Get TS and QM Files
        QStringList getTsFiles(const QString &thisFolder);               //!< get Ts Files
        QStringList getQmFiles(const QString &thisFolder);               //!< get Qm Files
        // Translation Source
        void setTranslationSource(const QString &thisTranslationSource); //!< set Translation Source
        QString getTranslationSource();                                  //!< get Translation Source
        // Translation File Prefix
        void setTransFilePrefix(const QString &thisTransFilePrefix);     //!< set TransFile Prefix
        QString getTransFilePrefix();                                    //!< get TransFile Prefix
        // Help Source
        void setHelpSource(const QString &thisHelpSource);               //!< set Help Source
        QString getHelpSource();                                         //!< get Help Source
        // Language Name
        QString getLanguageName();                                       //!< get Language Name
        void setLanguageName(const QString &thisLanguageName);           //!< set Language Name
        // Default Language Name and Code
        QString getDefaultLanguageName();                                //!< get Default Lanuage Name
        QString getDefaultLanguageCode();                                //!< get Default Language Code
        // Localize File Utilities
        QString getLanguageFromFile(const QString &thisPrefix, const QString &thisQmLanguageFile); //!< get Language From File
        QString getLocalizerCode(const QString &thisPrefix, const QString &thisQmFile);            //!< get Localizer Code
        QString getLocalizedCodeFromFile(const QString &thisPrefix, const QString &thisQmFile);    //!< get Localized Code From File
        QString getLanguageFile(const QString &thisLanguageCode, const QString &thisPath, const QString &thisPrefix); //!< get Language File
        // languageMatch
        bool languageMatch(const QString &thisPrefix, const QString &thisLang, const QString &thisQmFile); //!< language Match
        // language
        QString language(const QLocale &thisLocale);     //!< language
        QString languageName(const QLocale &thisLocale); //!< language Name used for tr to translate all names
        // Is Debug Message
        void setDebugMessage(bool thisState);                       //!< set Debug Message
        bool getDebugMessage();                                     //!< get Debug Message
        // Main Loaded
        bool getMainLoaded();                                       //!< get Main Loaded
        void setMainLoaded(bool thisMainLoaded);                    //!< set Main Loaded
        // Language
        void readLanguage();                                        //!< read Language
        void writeLanguage();                                       //!< write Language
        QLocale localeLanguage(const QString &thisLocaleCode);        //!< locale Language
        // DefaultDateFormat
        QString getDefaultDateFormat();                                  //!< get Default Date Format
        void setDefaultDateFormat(const QString &thisDefaultDateFormat); //!< set Default Date Format
        // translate This
        // Languages
        Q_INVOKABLE void setLanguages();                                //!< set Languages
        Q_INVOKABLE bool setLanguage(const QString &thisLanguageName);  //!< load Language

        Q_INVOKABLE QString translateThis(const QString &thisPhrase); //!< translate This is required for Translation Files
        Q_INVOKABLE QString getLangCode(const QString &thisString);   //!< get Lang Code
        Q_INVOKABLE QString translateInt(const QString &thisInt);     //!< translate Int
        Q_INVOKABLE QString translateDouble(const QString &thisDouble); //!< translate Floating point number
        Q_INVOKABLE QString translateShortMonth(int thisMonth);       //!< translate Short Month
        Q_INVOKABLE QString translateLongMonth(int thisMonth);        //!< translate Long Month
        Q_INVOKABLE QString translateShortWeek(int thisWeek);         //!< translate Short Week
        Q_INVOKABLE QString translateLongWeek(int thisWeek);          //!< translate Long Week
        Q_INVOKABLE QString translateTime(const QString &thisTime, const QString &thisTimeFormat);   //!< translate Time
        // language Code To Name and Name to Code
        Q_INVOKABLE QString languageCodeToName(const QString &thisLangageCode); //!< language Code To Name
        Q_INVOKABLE QString languageNameToCode(const QString &theLanguageName); //!< language Name to Code
        //
        #ifdef MY_QML
        // Singleton
        static QObject *qmlInstance(QQmlEngine *, QJSEngine *);
        #endif
        // Saved Language Name
        QString getSavedLanguageName();                            //!< get Saved Language Name
        QString getLanguageCode();                                 //!< get Language Code
        const QStringList &getLanguageCodeList() const;            //!< get Language Codes List
        const QStringList &getLanguageList() const;                //!< get Language List
        //
        // Crypto for Password
        Q_INVOKABLE QString encryptThis(const QString &thisSecret);           //!< encrypt This
        Q_INVOKABLE QString decryptThis(const QString &thisSecret);           //!< decrypt This
        //
        // Read/Write
        Q_INVOKABLE QString readSettings(const QString &thisSetting, const QString &thisDefault);   //!< read Settings
        Q_INVOKABLE bool writeSettings(const QString &thisSetting, const QString &thisValue);       //!< write Settings

        //
        void getCpuTemperature();
        //
        QString temperatureUnit();

        bool systemTrayEnabled();

    public slots:
        // Saved Language Name
        Q_INVOKABLE void setSavedLanguageName(const QString &thisLanguageName);
        // Language Code
        Q_INVOKABLE void setLanguageCode(const QString &thisLanguageCode);           //!< set Language Code
        // Language List
        Q_INVOKABLE void setLanguageList(const QStringList &thisLanguageList); //!< set Language List
        // Language Codes
        Q_INVOKABLE void setLanguageCodeList(const QStringList &thisLanguageCodesList); //!< set Language Codes List
        #ifdef MY_SYSTEM_TRAY
        //
        void hideIconTray();
        /* The slot that will accept the signal from the event click on the application icon in the system tray */
        void iconActivated(QSystemTrayIcon::ActivationReason reason);
        #endif
        // read-only property
        Q_INVOKABLE void setSystemTrayEnabled(bool isEnabled);
        Q_INVOKABLE void setTemperatureUnit(const QString &thisTemperature);

    signals:
        void languageChanged(QString thisLanguageName);          //!< language Changed Event
        void languageCodeChanged(QString thisLanguageCode);      //!< language Code Changed
        void savedLanguageNameChanged(QString thisLanguageName); //!< saved Language Name Changed
        void languageListChanged();                              //!< language List Changed
        void languageCodeListChanged();
        void temperatureUnitChanged();
        void systemTrayEnabledChanged();
        #ifdef MY_SYSTEM_TRAY
        void signalIconActivated();
        void signalShow();
        void signalQuit();
        #endif
        void encryptChanged();
        void settingsChanged();

    protected:
        QHash<int, QByteArray> roleNames() const; //!< roleNames

    private:
        QList<MyLanguageData> myItems;                  //!< \c myItems              @brief my Items
        static const QMap<QString, QString>  staticGenericLanguageCodeToName; //!< \c s_genericLanguageCodeToName @brief generic Language Code to Name
        static const QMap<QString, QString>  staticGenericLanguageNameToCode; //!< \c s_genericLanguageNameToCode @brief generic Language Name to Code
        static const QMap<QString, QString>  staticGenericLanguageNameToDateTimeFormat; //!< \c s_genericLanguageNameToDateTimeFormat @brief generic Language Name to Date Time Format
        QTranslator  *myTranslator = nullptr;           //!< \c myTranslator         @brief Translator
        QString       myTranslationSource  = "";        //!< \c myTranslationSource  @brief Translation Source
        QString       myHelpSource         = "";        //!< \c myHelpSource         @brief Help Source
        QString       myTransFilePrefix    = "";        //!< \c myTransFilePrefix    @brief Translation File Prefix
        QString       myLanguageCode       = "";        //!< \c myLanguageCode       @brief Two Digit Language Code
        QString       myLanguageName       = "";        //!< \c myLanguageName       @brief Language Name
        QString       myDefaultDateFormat  = "";        //!< \c myDefaultDateFormat  @brief my Default Date Format
        QMap<QString, QString> myCurrentQmFiles;        //!< \c myCurrentQmFiles     @brief my Current Qm Files
        QStringList   myLanguages;                      //!< \c myLanguages          @brief my Languages
        QStringList   myLanguageCodes;                  //!< \c myLanguageCodes      @brief my Language Codes
        bool          isDebugMessage       = true;     //!< \c isDebugMessage       @brief true of false for Debugging.
        bool          isMainLoaded         = false;     //!< \c isMainLoaded         @brief Set true after one shot time loads.
        QString       myTemperture;
        QString       myLastCrypted;
        bool          isSystemTrayEnalbe;
        #ifdef MY_SYSTEM_TRAY
        /* Declare the object of future applications for the tray icon*/
        QSystemTrayIcon    *myTrayIcon;
        #endif
        QString             myAppPath;
};
#endif // MyLanguageMODEL_H
/******************************* End of File *********************************/
