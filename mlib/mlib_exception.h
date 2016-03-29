#ifndef mlib_exception_h
#define mlib_exception_h

#include "mlib_log.h"
#include "mlib_helpers.h"

#include <exception>
#include <string>
#include <sstream>

MLIB_NS_BEGIN

class MException : std::exception
{
public:
    MException(const std::string& msg) : _msg(msg) {}
    
    virtual const char* what() const noexcept { return _msg.c_str(); };
    
private:
    std::string     _msg;
};

#define M_THROW(msg) \
{\
std::stringstream __ss__throw; __ss__throw << msg; M_ERROR("Exception: " << __ss__throw.str()); throw MLIB_NS::MException(__ss__throw.str()); \
}
    


MLIB_NS_END


#endif // mlib_exception_h