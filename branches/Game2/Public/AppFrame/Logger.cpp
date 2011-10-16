#include "stdafx.h"
#include "Logger.h"

#include "Alloc.h"

namespace AppFrame {

Logger::Logger()
{
    m_log = &AddLog("Logger.log", Standard, true);
}

Logger::~Logger()
{
    LogList::iterator itr = m_logs.begin();
    for (; itr != m_logs.end(); ++itr)
    {
        GAME_OBJ_DELETE(itr->second);
    }
}

Log& Logger::AddLog(const String &name,
                    LogLevel level/* = LOG_LEVEL_STANDARD*/,
                    bool current/* = false*/)
{
    LogList::iterator itr = m_logs.find(name);
    if (itr != m_logs.end())
    {
        AssertFalse(itr->second);
        return *(itr->second);
    }

    Log* log;
    GAME_OBJ_NEW(log, Log(name, level));
    AssertFalse(log);

    if (current || 0 == m_currentLog)
    {
        m_currentLog = log;
    }

    m_logs.insert(LogList::value_type(name, log));
    return *log;
}

bool Logger::DelLog(const String &name)
{
    LogList::iterator itr = m_logs.find(name);
    if (itr != m_logs.end())
    {
        if (itr->second == m_currentLog)
        {
            m_currentLog = 0;
        }

        GAME_OBJ_DELETE(itr->second);
        m_logs.erase(itr);

        return true;
    }

    return false;
}

bool Logger::DelLog(Log* log)
{
    AssertFalse(log);
    return DelLog(log->Name());
}

Log* Logger::GetLog(const String &name)
{
    LogList::iterator itr = m_logs.find(name);
    if (itr != m_logs.end())
    {
        AssertFalse(itr->second);
        return itr->second;
    }
    else
        return 0;
}

}; // namespace AppFrame
