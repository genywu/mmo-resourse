// -*- C++ -*-

/**
 * @file    Log.h
 * @date    Wed Nov 18 15:37:18 CST 2009
 * @author  Fox(yulefox.at.gmail.com)
 */

#pragma once

#include "Def.h"

namespace AppFrame {

enum LogLevel
{
    Error,       ///< only errors can be logged, `Err` is recommended 
    Warning,     ///< warnings can also be logged
    Standard,    ///< (default)
    Info,        ///< verbose
    Insane,      ///< only used when debuging
};

enum LogFlag
{
    Endf,        ///< Ends current log line and flushes to file.
    Endl,        ///< Ends current log line without flushing.
};

/**
 * The macro is used for catching details (file, funciton, line) when error
 * occured. ErrorInfo can be treat like LogLevel enum.
 * @usage log << Err << ....;
 */
#define Err                                     \
    Error << "(" __FILE__ << " "                \
    << __FUNCTION__ << ", "                     \
    << __LINE__ << ")\r\n    "

class LogListener;

/**
 * @class   Log
 * @brief   Base class of logging message for application.
 *
 * @remark ofstream vs. ostringstream
 * Without openning files, ofstream costs less time(Ubuntu 9.10, linux
 * 2.6.31-15-generic, Inter Core2 Quad Q8200, 3GiB RAM). Thus ofstream is
 * used to cache messages.
 * @remark Log is suggested being @a private for users.
 * through singleton Logger. 
 * e.g. log& l = GetLogger()->GetLog(XXX);
 */

class Log
{
public:
    /// Get the level of the log detail.
    LogLevel Level() const { return m_level; }

    /// Set the level of the log detail.
    /// @remark Reset m_level is allowed. However, previous messages which
    /// level are greater than old m_level would be ignored. It has no
    /// retroaction.
    void Level(LogLevel level) { m_level = level; }

    /// Get the level of current log stream.
    LogLevel CurLevel() const { return m_curLevel; }

    /// Get the name of the log.
    /// @remark Renamed log is disallowed.
    const String& Name() const { return m_name; }

    /// Register a listener to this log
    /// @param listener: a valid listener derived class
    void RegisteListener(void(*listener)(const String&))
    {
        m_listener = listener;
    }

    /// If current log stream is fresh, sets the level of it.
    Log& operator<<(LogLevel l)
    {
        if (m_fresh)
        {
            m_curLevel = l;
            if (m_curLevel <= m_level)
            {
                InitStream();
            }
        }
        return *this;
    }

    /// The exact method logging messages.
    template<class Type>
    Log& operator<<(const Type& v)
    {
        if (m_curLevel <= m_level)
        {
            if (m_fresh)
            {
                InitStream();
            }
            m_stream << v;
        }
        return *this;
    }

    /// Insert log flags.
    Log& operator<<(LogFlag f);


protected:
    friend class Logger;

    /// Default ctor is disallowed.
    Log();

    /// Protected ctor - only can be created by Logger.
    /// @param name
    /// Name of log file.
    /// @param cache
    /// If true, messages would be saved in m_stream instead of written
    /// to m_file till timeout (e.g. 60sec.). All unsaved messages would
    /// be lost if the application dumped. Otherwise, m_file would be
    /// flushed each time.
    /// @param level
    /// LogLevel
    Log(const String& name,
        LogLevel level = Standard)
        : m_fresh(true)
        , m_level(level)
        , m_curLevel(Standard)
        , m_name(name)
        , m_listener(NULL)
    {
        m_file.open(name.c_str(), ios_base::out|ios_base::app);
        AssertFalse(m_file.is_open());
    }

    ~Log()
    {
        FlushFile();
        m_file.close();
    }

    void FiniStream() {
        if (m_listener)
            m_listener(m_stream.str());
        m_fresh = true;
        m_curLevel = Standard;
        m_stream.str("");
    }

    void InitStream();

    void CacheStream()
    {
        m_cache.push_back(std::make_pair(m_stream.str(), m_curLevel));
        FiniStream();
    }

    void FlushFile();

    typedef OFile LogFile;
    typedef String LogString;
    typedef OStringStream LogStream;
    typedef std::vector<std::pair<LogString, LogLevel> > LogCache;

    ///< current logging stream has no charactors
    bool m_fresh;

    ///< log level for the log file
    LogLevel m_level;

    ///< log level for current logging stream
    LogLevel m_curLevel;

    ///< name of the log file
    LogString m_name;

    ///< current file stream
    LogFile m_file;

    ///< current logging stream
    LogStream m_stream;

    ///< caching list
    LogCache m_cache;

    ///< listeners of current log
    void(*m_listener)(const String& log);
}; // class Log

}; // namespace AppFrame
