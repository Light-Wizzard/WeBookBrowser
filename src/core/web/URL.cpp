#include "public_suffix/PublicSuffixManager.h"
#include "URL.h"
/*****************************************************************************/
/**
 * @brief URL::URL Default constructor.
 */
URL::URL() :
    QUrl()
{
}
/*****************************************************************************/
/**
 * @brief URL::URL Constructs a copy of the given URL.
 * @param other
 */
URL::URL(const QUrl &other) :
    QUrl(other)
{
}
/*****************************************************************************/
/**
 * @brief URL::URL Constructs a URL from the given string a parsing mode.
 * @param url
 * @param parsingMode
 */
URL::URL(const QString &url, URL::ParsingMode parsingMode) :
    QUrl(url, parsingMode)
{
}
/*****************************************************************************/
/**
 * @brief URL::getSecondLevelDomain Returns the second-level domain of the URL (ex: websiteA.com; websiteB.co.uk).
 * @return
 */
QString URL::getSecondLevelDomain() const
{
    PublicSuffixManager &suffixManager = PublicSuffixManager::instance();
    const QString topLevelDomain = suffixManager.findTld(host().toLower());
    const QString host = this->host();

    if (topLevelDomain.isEmpty() || host.isEmpty())
        return QString();

    QString domain = host.left(host.size() - topLevelDomain.size());

    if (domain.count(QChar('.')) == 0)
        return host;

    while (domain.count(QChar('.')) > 1)
        domain = domain.mid(domain.indexOf(QChar('.')) + 1);

    return domain + topLevelDomain;
}
