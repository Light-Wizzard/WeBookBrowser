/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#include "browser.h"
//#include "webpopupwindow.h"
//#include "tabwidget.h"
#include "WebPage.h"
#include "WebView.h"
#include "MainWindow.h"
#include "ui_certificateerrordialog.h"
#include "ui_passworddialog.h"
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QAuthenticator>
#include <QTimer>
#include <QStyle>

/*****************************************************************************/
/**
 * @brief WebView::WebView
 * @param parent
 */
WebView::WebView(QWidget *parent) : QWebEngineView(parent), m_loadProgress(100)
{
    connect(this, &QWebEngineView::loadStarted,  [this]() { m_loadProgress = 0; emit favIconChanged(favIcon()); });
    connect(this, &QWebEngineView::loadProgress, [this](int progress)  { m_loadProgress = progress; });
    connect(this, &QWebEngineView::loadFinished, [this](bool success)  { m_loadProgress = success ? 100 : -1; emit favIconChanged(favIcon()); });
    connect(this, &QWebEngineView::iconChanged,  [this](const QIcon &) { emit favIconChanged(favIcon()); });

    connect(this, &QWebEngineView::renderProcessTerminated, [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode)
    {
        QString status;
        switch (termStatus)
        {
            case QWebEnginePage::NormalTerminationStatus:
                status = tr("Render process normal exit");
                break;
            case QWebEnginePage::AbnormalTerminationStatus:
                status = tr("Render process abnormal exit");
                break;
            case QWebEnginePage::CrashedTerminationStatus:
                status = tr("Render process crashed");
                break;
            case QWebEnginePage::KilledTerminationStatus:
                status = tr("Render process killed");
                break;
        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(), status, tr("Render process exited with code: %1\nDo you want to reload the page ?").arg(statusCode));
        if (btn == QMessageBox::Yes) { QTimer::singleShot(0, this, [this] { reload(); }); }
    });
}
/*****************************************************************************/
/**
 * @brief questionForFeature
 * @param feature
 * @return
 */
inline QString questionForFeature(QWebEnginePage::Feature feature)
{
    switch (feature)
    {
        case QWebEnginePage::Geolocation:              return QObject::tr("Allow %1 to access your location information?");
        case QWebEnginePage::MediaAudioCapture:        return QObject::tr("Allow %1 to access your microphone?");
        case QWebEnginePage::MediaVideoCapture:        return QObject::tr("Allow %1 to access your webcam?");
        case QWebEnginePage::MediaAudioVideoCapture:   return QObject::tr("Allow %1 to access your microphone and webcam?");
        case QWebEnginePage::MouseLock:                return QObject::tr("Allow %1 to lock your mouse cursor?");
        case QWebEnginePage::DesktopVideoCapture:      return QObject::tr("Allow %1 to capture video of your desktop?");
        case QWebEnginePage::DesktopAudioVideoCapture: return QObject::tr("Allow %1 to capture audio and video of your desktop?");
        case QWebEnginePage::Notifications:            return QObject::tr("Allow %1 to show notification on your desktop?");
    }
    return QString();
}
/*****************************************************************************/
/**
 * @brief WebView::setPage
 * @param page
 */
void WebView::setPage(WebPage *page)
{
    if (auto oldPage = qobject_cast<WebPage *>(QWebEngineView::page()))
    {
        disconnect(oldPage, &WebPage::createCertificateErrorDialog, this,  &WebView::handleCertificateError);
        disconnect(oldPage, &QWebEnginePage::authenticationRequired, this, &WebView::handleAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::featurePermissionRequested, this, &WebView::handleFeaturePermissionRequested);
        disconnect(oldPage, &QWebEnginePage::proxyAuthenticationRequired, this, &WebView::handleProxyAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::registerProtocolHandlerRequested, this, &WebView::handleRegisterProtocolHandlerRequested);
    }
    createWebActionTrigger(page,QWebEnginePage::Forward);
    createWebActionTrigger(page,QWebEnginePage::Back);
    createWebActionTrigger(page,QWebEnginePage::Reload);
    createWebActionTrigger(page,QWebEnginePage::Stop);
    QWebEngineView::setPage(page);
    connect(page, &WebPage::createCertificateErrorDialog, this, &WebView::handleCertificateError);
    connect(page, &QWebEnginePage::authenticationRequired, this, &WebView::handleAuthenticationRequired);
    connect(page, &QWebEnginePage::featurePermissionRequested, this, &WebView::handleFeaturePermissionRequested);
    connect(page, &QWebEnginePage::proxyAuthenticationRequired, this, &WebView::handleProxyAuthenticationRequired);
    connect(page, &QWebEnginePage::registerProtocolHandlerRequested, this, &WebView::handleRegisterProtocolHandlerRequested);
}
/*****************************************************************************/
/**
 * @brief WebView::loadProgress
 * @return
 */
int WebView::loadProgress() const
{
    return m_loadProgress;
}
/*****************************************************************************/
/**
 * @brief WebView::createWebActionTrigger
 * @param page
 * @param webAction
 */
void WebView::createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction] { emit webActionEnabledChanged(webAction, action->isEnabled()); });
}
/*****************************************************************************/
/**
 * @brief WebView::isWebActionEnabled
 * @param webAction
 * @return
 */
bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}
/*****************************************************************************/
/**
 * @brief WebView::favIcon
 * @return
 */
QIcon WebView::favIcon() const
{
    QIcon favIcon = icon();
    if (!favIcon.isNull())
        return favIcon;

    if (m_loadProgress < 0)
    {
        static QIcon errorIcon(QStringLiteral(":dialog-error.png"));
        return errorIcon;
    }
    else if (m_loadProgress < 100)
    {
        static QIcon loadingIcon(QStringLiteral(":view-refresh.png"));
        return loadingIcon;
    }
    else
    {
        static QIcon defaultIcon(QStringLiteral(":text-html.png"));
        return defaultIcon;
    }
}
/*****************************************************************************/
//QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
//{
//    MainWindow *mainWindow = qobject_cast<MainWindow*>(window());
//    if (!mainWindow)
//        return nullptr;

//    switch (type)
//    {
//        case QWebEnginePage::WebBrowserTab:           { return mainWindow->tabWidget()->createTab(); }
//        case QWebEnginePage::WebBrowserBackgroundTab: { return mainWindow->tabWidget()->createBackgroundTab(); }
//        case QWebEnginePage::WebBrowserWindow:        { return mainWindow->browser()->createWindow()->currentTab(); }
//        case QWebEnginePage::WebDialog:
//            {
//                WebPopupWindow *popup = new WebPopupWindow(page()->profile());
//                connect(popup->view(), &WebView::devToolsRequested, this, &WebView::devToolsRequested);
//                return popup->view();
//            }
//    }
//    return nullptr;
//}
/*****************************************************************************/
/**
 * @brief WebView::contextMenuEvent
 * @param event
 */
void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    const QList<QAction *> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend())
    {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction *action = new QAction(menu);
        action->setText("Open inspector in new window");
        connect(action, &QAction::triggered, [this]() { emit devToolsRequested(page()); });

        QAction *before(inspectElement == actions.cend() ? nullptr : *inspectElement);
        menu->insertAction(before, action);
    }
    else
    {
        (*inspectElement)->setText(tr("Inspect element"));
    }
    menu->popup(event->globalPos());
}
/*****************************************************************************/
/**
 * @brief WebView::handleCertificateError
 * @param error
 */
void WebView::handleCertificateError(QWebEngineCertificateError error)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::CertificateErrorDialog certificateDialog;
    certificateDialog.setupUi(&dialog);
    certificateDialog.labelIcon->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, window()));
    certificateDialog.labelIcon->setPixmap(icon.pixmap(32, 32));
    certificateDialog.labelError->setText(error.description());
    dialog.setWindowTitle(tr("Certificate Error"));

    if (dialog.exec() == QDialog::Accepted)
        error.acceptCertificate();
    else
        error.rejectCertificate();
}
/*****************************************************************************/
/**
 * @brief WebView::handleAuthenticationRequired
 * @param requestUrl
 * @param auth
 */
void WebView::handleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.labelIcon->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, window()));
    passwordDialog.labelIcon->setPixmap(icon.pixmap(32, 32));

    QString introMessage(tr("Enter username and password for \"%1\" at %2").arg(auth->realm()).arg(requestUrl.toString().toHtmlEscaped()));
    passwordDialog.labelInfo->setText(introMessage);
    passwordDialog.labelInfo->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        auth->setUser(passwordDialog.lineEditUserName->text());
        auth->setPassword(passwordDialog.lineEditPassword->text());
    }
    else
    {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}
/*****************************************************************************/
/**
 * @brief WebView::handleFeaturePermissionRequested
 * @param securityOrigin
 * @param feature
 */
void WebView::handleFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
    QString title = tr("Permission Request");
    QString question = questionForFeature(feature).arg(securityOrigin.host());
    if (!question.isEmpty() && QMessageBox::question(window(), title, question) == QMessageBox::Yes)
        page()->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
    else
        page()->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
}
/*****************************************************************************/
/**
 * @brief WebView::handleProxyAuthenticationRequired
 * @param auth
 * @param proxyHost
 */
void WebView::handleProxyAuthenticationRequired(const QUrl &, QAuthenticator *auth, const QString &proxyHost)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.labelIcon->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, window()));
    passwordDialog.labelIcon->setPixmap(icon.pixmap(32, 32));

    QString introMessage = tr("Connect to proxy \"%1\" using:");
    introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
    passwordDialog.labelInfo->setText(introMessage);
    passwordDialog.labelInfo->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        auth->setUser(passwordDialog.lineEditUserName->text());
        auth->setPassword(passwordDialog.lineEditPassword->text());
    }
    else
    {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}
//! [registerProtocolHandlerRequested]
/*****************************************************************************/
/**
 * @brief WebView::handleRegisterProtocolHandlerRequested
 * @param request
 */
void WebView::handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request)
{
    auto answer = QMessageBox::question(window(), tr("Permission Request"), tr("Allow %1 to open all %2 links?").arg(request.origin().host()).arg(request.scheme()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
/******************************* End of File *********************************/
