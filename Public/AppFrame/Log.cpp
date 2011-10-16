#include "stdafx.h"
#include "Log.h"
#include "DateTime.h"

namespace AppFrame { 

Log& Log::operator<<(LogFlag f)
{
    if (m_curLevel <= m_level)
    {
        switch (f)
        {
        case Endf:
            m_stream << std::endl;
            FlushFile();
            break;

        case Endl:
            m_stream << std::endl;
            FlushFile();
            break;

        default:
            break;
        }
    }

    return *this;
}

void Log::InitStream()
{
    m_fresh = false;
    GetCurTimeString(m_stream);

    switch (m_curLevel)
    {
    case Error:
        m_stream << "(Error) ";
        break;

    case Warning:
        m_stream << "(Warn)  ";
        break;

    case Standard:
        m_stream << "(Std)   ";
        break;

    case Info:
        m_stream << "(Info)  ";
        break;

    case Insane:
        m_stream << "(Insan) ";
        break;

    default:
        m_stream << "(Unkwn) ";
        break;
    }
}

void Log::FlushFile()
{
    CacheStream();

    LogCache::iterator itr = m_cache.begin();

    for (; itr != m_cache.end(); ++itr)
    {
        if (m_level >= itr->second)
        {
            m_file << itr->first;
        }
    }
    
    m_file.flush();
    m_cache.clear();
}

}; // namespace AppFrame
