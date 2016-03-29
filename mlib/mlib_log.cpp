#include "mlib_log.h"

#include "mlib_assert.h"
#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <android/log.h>
#endif

MLIB_NS_BEGIN

static std::mutex g_mutex;

MLogger::MLogger(std::ostream& os) :
_os(os),
_shouldStripFilename(true),
_minLogLevel(
#ifdef DEBUG
kMLogLevelDebug
#else
kMLogLevelInfo
#endif
)
{
    
}

MLogger& MLogger::SharedInstance()
{
    static MLogger logger(std::cout);
    return logger;
}


void MLogger::log(mlib::MLogLevel level, const std::string &message, const char *filename, long line)
{
    if (level < _minLogLevel) return;
    
    
    if (_shouldStripFilename)
    {
        M_ASSERT(strrchr(filename, '/') != filename);

        // remove path component
        const char * lastSlash = strrchr(filename, '/');
        if (lastSlash)
        {
            filename = lastSlash + 1;
        }
    }
    
    time_t ts;
    time(&ts);
    struct tm now;
    localtime_r(&ts, &now);
    std::stringstream ss;
    
    timeval tv;
    gettimeofday(&tv, NULL);
    
    /*
     ss << (now.tm_year+1900)
     << std::setw(2) << std::setfill('0') << (now.tm_mon+1)
     << std::setw(2) << std::setfill('0') << now.tm_mday
     << " "
     ;
     //*/

    ss
    << std::setw(2) << std::setfill('0') << now.tm_hour
    << std::setw(2) << std::setfill('0') << now.tm_min
    << std::setw(2) << std::setfill('0') << now.tm_sec
    << "." << std::setw(3) << std::setfill('0') << tv.tv_usec / 1000
    << " | "
    ;
    
    switch (level)
    {
        case kMLogLevelDebug:
            ss << "   DEBUG: ";
            break;
        case kMLogLevelInfo:
            ss << "    INFO: ";
            break;
        case kMLogLevelWarning:
            ss << " WARNING: ";
            break;
        case kMLogLevelError:
            ss << "   ERROR: ";
            break;
        case kMLogLevelFatal:
            ss << " *FATAL*: ";
            break;
        default:
            ss << "   TRACE: ";
            break;
    }

    //*
    ss
    << " {" << std::ios::hex << std::this_thread::get_id() << "}"
    ;
     //*/
    ss
    << filename << "[" << line << "]"
    << ": " << message
    << std::endl;
    

    std::lock_guard<decltype(g_mutex)> guard(g_mutex);
    
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    _os << ss.str();
    _os.flush();
#else
    __android_log_print(ANDROID_LOG_DEBUG, "MLogger debug",  ss.str().c_str());
#endif
}

MLIB_NS_END
