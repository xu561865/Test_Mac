#ifndef mlib_log_h
#define mlib_log_h

#include "mlib_helpers.h"

#include <string>
#include <sstream>
#include <ostream>

MLIB_NS_BEGIN

enum MLogLevel
{
    kMLogLevelTrace = 0,
    kMLogLevelDebug = 10,
    kMLogLevelInfo = 20,
    kMLogLevelWarning = 30,
    kMLogLevelError = 40,
    kMLogLevelFatal = 50,
};

class MLogger
{
public:
    static MLogger& SharedInstance();
    
    MLogger(std::ostream & os);
    void log(MLogLevel level, const std::string & message, const char * filename, long line);
    
public:
    std::ostream&   _os;
    bool            _shouldStripFilename;
    MLogLevel       _minLogLevel;
    
};

#define __M_MAKE_LOG(level, msg) \
{\
    std::stringstream ss; ss << msg; MLIB_NS::MLogger::SharedInstance().log(level, ss.str(), __FILE__, __LINE__); \
}

#ifdef DEBUG
#define M_TRACE(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelTrace, msg)
#define M_DEBUG(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelDebug, msg)
#else
#define M_TRACE(msg)
#define M_DEBUG(msg)
#endif

#define M_INFO(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelInfo, msg)
#define M_WARNING(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelWarning, msg)
#define M_ERROR(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelError, msg)
#define M_FATAL(msg) __M_MAKE_LOG(MLIB_NS::kMLogLevelFatal, msg)

MLIB_NS_END



#endif // mlib_log_h
