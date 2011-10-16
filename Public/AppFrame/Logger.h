// -*- C++ -*-

/**
 * @file    Logger.h
 * @date    Tue Nov 17 14:33:55 CST 2009
 * @author  Fox(yulefox.at.gmail.com)
 */

#pragma once

#include "Log.h"

namespace AppFrame {

/**
 * @class   Logger
 * @brief   Manages all Log objects.
 *          It is used to create, operate, destroy all Log objects.
 */

class Logger
{
public:
    Logger();
    ~Logger();

    /// Creates and adds a new Log with given name.
    /// @param name
    /// Name of new log, e.g. 'Fox.log'. If existed, it creates no log and
    /// return reference to the existed log.
    /// @param current
    /// If true or m_currentLog is 0, the creating log is set to be current
    /// one.
    /// @param cache
    /// If true, message will be cached without written to file at once.
    /// And you need to listen it with some a LogListener, so
    /// message is not lost.
    /// @param level
    /// LogLevel.
    /// @remark
    /// You would better add a named log first when you want to log
    /// something. And check whether has a same name log yourself.
    Log& AddLog(const String& name,
                LogLevel level = Standard,
                bool current = false);

    /// Closes and removes a named log.
    bool DelLog(const String& name);

    /// Closes and removes a log.
    bool DelLog(Log* log);

    /// Retrieves a named log.
    Log* GetLog(const String& name);

    /// Retrieves current log.
    Log* CurrentLog() const { return m_currentLog; }

    /// Set the given log to be current one.
    /// @returns The previous 'current log'. 
    Log* CurrentLog(Log* log)
    {
        Log* old = m_currentLog;
        m_currentLog = log;
        return old;
    }

    template<class Type>
    Logger& operator<<(const Type& v)
    {
        if (m_currentLog != 0)
        {
            *m_currentLog << v;
        }
        else
        {
            std::cout << v;
        }
        return *this;
    }

protected:
    typedef std::map<String, Log*, std::less<String> > LogList;

    /// Manipulate all Log.
    LogList m_logs;

    /// Named a current Log for convenience.
    Log* m_currentLog;

    /// Log for log.
    Log* m_log;
}; // class Logger

#define GetLogger() GetInst(Logger)

}; // namespace AppFrame
