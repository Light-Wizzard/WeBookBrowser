#include <QApplication>
// FIXME MY_SYSTEM_TRAY
#ifdef MY_SYSTEM_TRAY
    #ifndef QT_NO_SYSTEMTRAYICON
        #include <QSystemTrayIcon>
    #endif
#endif
#include <QDateTime>
#include <QStandardPaths>
#include <QFileInfo>
#include <QIcon>
#include <QStyleHints>
#include <QUrl>
#include <QScreen>
#ifdef MY_QML
    //#include <QGuiApplication>
    #include <QQmlApplicationEngine>
    #include <QQmlContext>
    #include <QtQuick/QQuickItem>
#else
    #include "MainWindow.h"
#endif
// Weather
#include <QLoggingCategory>
//
#include "src/MyLanguageModel.h"
//#include "src/WeatherAppModel.h"
//#include "src/MyWebContent.h"
// STD
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
//
#if Q_OS_MSDOS || defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    #define VERSION "1.0.0"
#else
    #define VERSION "1.0"
#endif

#ifndef Q_OS_WIN
    #include <stdio.h>
    #include <stdlib.h>
#endif


#ifdef Q_OS_LINUX

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <string.h>

#define BT_BUF_SIZE 100
/*****************************************************************************/
/**
 * @brief MyCrashHandler
 * @param s
 */
void MyCrashHandler(int s, siginfo_t * /*siginfo*/, void * /*context*/)
{
    if (s != SIGSEGV)
        return;
    void *buffer[BT_BUF_SIZE];

    int nptrs = backtrace(buffer, BT_BUF_SIZE);
    char **strings = backtrace_symbols(buffer, nptrs);

    if (strings == NULL)
    {
        std::cout << "Could not get backtrace symbols" << std::endl;
        exit(EXIT_FAILURE);
    }

    QDir d(QString("%1/.config/Vaccarelli").arg(QDir::homePath()));
    if (!d.cd(QLatin1String("crash-logs")))
    {
        if (!d.mkdir(QLatin1String("crash-logs")) || !d.cd(QLatin1String("crash-logs")))
        {
            std::cout << "Could not create crash log directory" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    QDateTime now = QDateTime::currentDateTime();
    QFile crashFile(QString("%1/Crash_%2.log").arg(d.absolutePath(), now.toString(Qt::ISODate)));
    if (crashFile.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&crashFile);
        for (int i = 0; i < nptrs; ++i)
        {
            stream << "[" << i << "]: " << strings[i] << "\n";
        }

        crashFile.close();

        std::cout << "Saved crash log to " << QFileInfo(crashFile).absoluteFilePath().toStdString() << std::endl;
    }
    else
        std::cout << "Could not save crash log to file" << std::endl;

    free(strings);

    exit(EXIT_FAILURE);
}
#undef BT_BUF_SIZE
#endif
//
QString myLogFile = "";
/*****************************************************************************/
/**
 * @brief getMessageLogFile
 * @param thisAppDataLocation
 * @return
 */
QString getMessageLogFile(const QString &thisAppDataLocation)
{
    return QString("%1%2%3").arg(thisAppDataLocation, QDir::separator(), "messageLog.txt");
}
/*****************************************************************************/
/**
 * @brief getFileErrorMessage
 * @param thisAppDataLocation
 * @return
 */
QString getFileErrorMessage(const QString &thisAppDataLocation)
{
    return QString("%1: %2").arg(QObject::tr("Failed to open log file"), thisAppDataLocation);
}
/*****************************************************************************/
/**
 * @brief setMessageLogFile
 * @param thisAppName
 * @param thisAppDataLocation
 * @return
 */
bool setMessageLogFile(const QString &thisAppName, const QString &thisAppDataLocation)
{
    QString theFullFilePath = getMessageLogFile(thisAppDataLocation);
    if (QFileInfo::exists(theFullFilePath) && QFileInfo(theFullFilePath).isFile())
    { QFile::remove(theFullFilePath); }
    QFile theFile(theFullFilePath);
    if(!theFile.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate))
    {
        QString theErrorMessage = QString("%1: %2").arg(QObject::tr("Could not open log file"), theFullFilePath);
        std::cout << "setMessageLogFile(" << thisAppName.toStdString() << "," << thisAppDataLocation.toStdString() << ") -> " << theErrorMessage.toStdString() << std::endl;
        return false;
    }
    // Write to log file
    QTextStream theFileStream(&theFile);
    const QDateTime theDateTimeStamp = QDateTime::currentDateTime();
    // FIXME "dd MMM yyyy hh:mm:ss"
    QString theDateStamp = QString("%1: %2 - %3 %4").arg(QObject::tr("Log File"), thisAppName, theDateTimeStamp.toString("dd MMM yyyy hh:mm:ss"), theDateTimeStamp.timeZoneAbbreviation());
    theFileStream << theDateStamp  << '\n';
    theFile.flush();
    theFile.close();
    if (QFileInfo::exists(theFullFilePath) && QFileInfo(theFullFilePath).isFile())
    {
        myLogFile = theFullFilePath;
        return true;
    }
    else
        { return false; }
}
/*****************************************************************************/
/**
 * @brief logEvents
 * @param thisMessage
 */
void logEvents(const QString &thisMessage)
{
    QFile theFileHandle(myLogFile);
    if(!theFileHandle.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ))
    {
        // Do not use qDebug() since that is a loop
        std::cout << "logEvents(" << thisMessage.toStdString() << ") -> " << QObject::tr("Log File failed to open").toStdString() << " " << QObject::tr("Log File=|").toStdString() << myLogFile.toStdString() << "|" << std::endl;
        return;
    }
    QTextStream theFileStream(&theFileHandle);
    theFileStream << thisMessage;
    theFileStream.flush();
    theFileHandle.close();
}
/*****************************************************************************/
/**
 * @brief myMessageHandler I use a special Message Handler to format the output of Error.
 * @param thisType
 * @param thisContext
 * @param thisMsg
 */
void myMessageHandler(QtMsgType thisType, const QMessageLogContext &thisContext, const QString &thisMsg)
{
    QByteArray theLocalMsg  = thisMsg.toLocal8Bit();
    const char *theFile     = thisContext.file ? thisContext.file : "";
    const char *theFunction = thisContext.function ? thisContext.function : "";
    //
    switch (thisType)
    {
      case QtDebugMsg:
        logEvents(QString("Debug: %1 (%2:%3, %4)\n").arg(theLocalMsg.constData(), theFile, QString::number(thisContext.line), theFunction));
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", theLocalMsg.constData(), theFile, thisContext.line, theFunction);
        break;
      case QtInfoMsg:
        logEvents(QString("Info: %1 (%2:%3, %4)\n").arg(theLocalMsg.constData(), theFile, QString::number(thisContext.line), theFunction));
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", theLocalMsg.constData(), theFile, thisContext.line, theFunction);
        break;
      case QtWarningMsg:
        logEvents(QString("Warning: %1 (%2:%3, %4)\n").arg(theLocalMsg.constData(), theFile, QString::number(thisContext.line), theFunction));
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", theLocalMsg.constData(), theFile, thisContext.line, theFunction);
        break;
      case QtCriticalMsg:
        logEvents(QString("Critical: %1 (%2:%3, %4)\n").arg(theLocalMsg.constData(), theFile, QString::number(thisContext.line), theFunction));
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", theLocalMsg.constData(), theFile, thisContext.line, theFunction);
        break;
      case QtFatalMsg:
        logEvents(QString("Fatal: %1 (%2:%3, %4)\n").arg(theLocalMsg.constData(), theFile, QString::number(thisContext.line), theFunction));
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", theLocalMsg.constData(), theFile, thisContext.line, theFunction);
        break;
    }
}
/*****************************************************************************/
/**
 * @brief isFileExists
 * @param thisFile
 * @return
 */
bool isFileExists(const QString &thisFile)
{
    // check if file exists and if yes: Is it really a file and not directory?
    return QFileInfo::exists(thisFile) && QFileInfo(thisFile).isFile();
} // end isFileExists
/*****************************************************************************/
/**
 * @brief getAppDataLocation
 * @return
 */
QString getAppDataLocation()
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

    QDir dir(theAppDataLocation);
    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        {
            // Error FIXME
        }
    }
    QString theFileName = QString("%1%2%3").arg(theAppDataLocation, QDir::separator(), "ReadMe.txt");
    if (!isFileExists(theFileName))
    {
        QFile theFile(theFileName);
        if (theFile.open(QIODevice::WriteOnly))
        {
            /* Point a QTextStream object at the file */
            QTextStream outStream(&theFile);
            /* Write the line to the file */
            outStream << QString("%1 %2").arg(QObject::tr("This file was created in main to test if this folder is writeable by"), QCoreApplication::applicationName()).toLocal8Bit();
            /* Close the file */
            theFile.close();
            if (!isFileExists(theAppDataLocation))
            {
                // Error FIXME
            }
        }
    }
    return theAppDataLocation;
} // end getAppDataLocation
/*****************************************************************************/
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules("wapp.*.debug=false");
    QString theAppName = "WeBookBrowser";
    QString theAppDisplayName = "WeBookBrowser";
    //bool isDebugMessage = true;
    //
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //
    #ifdef MY_QML
    // This is not a normal QML app: QGuiApplication theApplication(argc, argv);
    QApplication theApplication(argc, argv);
    QQmlApplicationEngine theEngine;
    //
    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", theAppDisplayName.toLocal8Bit()));
    #else
    // Load Resource File QtLingo.qrc before creating Application
    Q_INIT_RESOURCE(WeBookBrowser);
    QApplication theApplication(argc, argv);
    //
    #ifdef MY_SYSTEM_TRAY
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(nullptr, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    #endif
    #ifdef MY_LANGUAGE_MODEL
    MyLanguageModel *theLanguageModel = new MyLanguageModel(qApp);
    #endif
    #endif
    theApplication.setWindowIcon(QIcon(":/images/logo.png"));
    // Setup the Application for MyOrgSettings
    theApplication.setOrganizationName(theAppName);
    theApplication.setApplicationName(theAppName);
    theApplication.setApplicationDisplayName(theAppDisplayName);
    theApplication.setApplicationVersion(VERSION);
    //
    // Run after the Application and setMessageLogFile ran
    #ifndef Q_OS_WIN
    qInstallMessageHandler(myMessageHandler);
    #endif
    // FIXME what if error
    setMessageLogFile(theApplication.applicationName(), getAppDataLocation());
    #ifdef Q_OS_LINUX
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &MyCrashHandler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &act, NULL) < 0)
        qWarning() << "Could not install handler for signal SIGSEGV";
    #endif
    //
    #ifdef MY_QML
    qmlRegisterSingletonType<MyLanguageModel>("LanguageModel", 1, 0, "LanguageModel", MyLanguageModel::qmlInstance);
    //
    qmlRegisterSingletonType<MyWebContent>("MyWebContent", 1, 0, "MyWebContent", MyWebContent::qmlInstance);
    //
    #ifdef MY_WEATHER
    qmlRegisterType<WeatherData>("WeatherInfo", 1, 0, "WeatherData");
    qmlRegisterType<AppModel>("WeatherInfo", 1, 0, "AppModel");
    qRegisterMetaType<WeatherData>();
    #endif
    //
    const QUrl theQmlUrl(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&theEngine, &QQmlApplicationEngine::objectCreated, &theApplication, [theQmlUrl](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && theQmlUrl == objUrl)
            { QCoreApplication::exit(-1); }
    }, Qt::QueuedConnection);
    theEngine.load(theQmlUrl);
    if (theEngine.rootObjects().isEmpty()) return -1;

    #else
        #ifdef MYLANG
            MainWindow *theMainWindow = new MainWindow(theLanguageModel);
        #else
            MainWindow *theMainWindow = new MainWindow();
            #ifdef MY_LANGUAGE_MODEL
            theMainWindow->setLanguageModel(theLanguageModel);
            #endif
        #endif
        theMainWindow->show();
    #endif
    return theApplication.exec();
}
/******************************* End of File *********************************/
