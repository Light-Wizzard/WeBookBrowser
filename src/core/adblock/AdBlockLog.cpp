#include "AdBlockLog.h"

#include <algorithm>

namespace adblock
{
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::AdBlockLog Constructs the log with a given parent.
     * @param parent
     */
    AdBlockLog::AdBlockLog(QObject *parent) : QObject(parent), m_entries(), m_timerId(0)
    {
        // Prune logs every 30 minutes
        m_timerId = startTimer(1000 * 60 * 30);
    }
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::~AdBlockLog Logging destructor.
     */
    AdBlockLog::~AdBlockLog()
    {
        killTimer(m_timerId);
    }
    /*****************************************************************************/
    /**
     * @brief addEntry Adds a network action performed by the ad block system to the logs
     * @param action The action that was done to the request
     * @param firstPartyUrl The source from which the request was made
     * @param requestUrl The resource that was requested
     * @param resourceType The type or types associated with the requested resource
     * @param rule The filter rule that was applied to the request
     * @param timestamp The time of the network action
     */
    void AdBlockLog::addEntry(FilterAction action, const QUrl &firstPartyUrl, const QUrl &requestUrl,
                              ElementType resourceType, const QString &rule, const QDateTime &timestamp)
    {
        auto it = m_entries.find(firstPartyUrl);
        if (it != m_entries.end())
            it->push_back({ action, firstPartyUrl, requestUrl, resourceType, rule, timestamp });
        else
            m_entries[firstPartyUrl] = { { action, firstPartyUrl, requestUrl, resourceType, rule, timestamp } };
    }
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::getAllEntries Returns all log entries.
     * @return
     */
    std::vector<LogEntry> AdBlockLog::getAllEntries() const
    {
        // Combine all entries
        std::vector<LogEntry> entries;
        for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it)
        {
            for (const LogEntry &logEntry : *it)
                entries.push_back(logEntry);
        }

        // Sort entries from newest to oldest
        std::sort(entries.begin(), entries.end(), [](const LogEntry &a, const LogEntry &b){
            return a.Timestamp < b.Timestamp;
        });

        return entries;
    }
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::getEntriesFor Returns all log entries associated with the given first party request url, or an empty container if no entries are found.
     * @param firstPartyUrl
     * @return
     */
    const std::vector<LogEntry> &AdBlockLog::getEntriesFor(const QUrl &firstPartyUrl)
    {
        auto it = m_entries.find(firstPartyUrl);
        if (it != m_entries.end())
            return *it;

        std::vector<LogEntry> entriesForUrl;
        it = m_entries.insert(firstPartyUrl, entriesForUrl);
        return *it;
    }
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::timerEvent Called on a regular interval to prune older log entries.
     */
    void AdBlockLog::timerEvent(QTimerEvent */*event*/)
    {
        pruneLogs();
    }
    /*****************************************************************************/
    /**
     * @brief AdBlockLog::pruneLogs Removes any entries from the logs that are more than 30 minutes old.
     */
    void AdBlockLog::pruneLogs()
    {
        const quint64 pruneThreshold = 1000 * 60 * 30;
        const QDateTime now = QDateTime::currentDateTime();
        auto removeCheck = [&](const LogEntry &logEntry) {
            return static_cast<quint64>(logEntry.Timestamp.msecsTo(now)) >= pruneThreshold;
        };

        for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
        {
            std::vector<LogEntry> &entries = *it;
            auto newEnd = std::remove_if(entries.begin(), entries.end(), removeCheck);
            entries.erase(newEnd, entries.end());
        }
    }
}
