

#ifndef __LSocketErrorHandler__
#define __LSocketErrorHandler__

#include <iostream>
#include "json.h"

class LSocketErrorHandler
{
public:
    
    typedef enum {
        // client error
        ERROR_PARSE_JSON = 60001,
        ERROR_DEFLATE_DATA = 60002,
        ERROR_NETWORK = 60003,
        
        // Reserved Constants
        ERR_SERVER_INTERNAL = 50001,
        ERR_MOD_NOT_FOUND = 50002,
        ERR_ACTION_NOT_SUPPORTED = 50003,
        ERR_MSG_NOT_SIGNED = 50004,
        ERR_TOKEN_EXPIRED = 50005,
        ERR_TOKEN_INVALID = 50006,
        ERR_SPEC_NOT_FOUND = 50007,
        ERR_SPEC_ERROR = 50008,
        ERR_REQ_ERROR = 50009,
        ERR_CLIENT_NEEDS_RESET = 50010, // nuclear weapon!!! be careful
        ERR_SPEC_EXPIRED = 50011,
        ERR_IAP_VERFICATION_FAILED = 50012,
        ERR_TOKEN_MISMATCH = 50013,
        ERR_SERVER_MESSAGE = 50014,
        ERR_CLIENT_NEEDS_UPDATE = 50015,
        
        
    } Errno_t;
    
    typedef enum
    {
        CB_SHUTDOWN = 1,
        CB_RESTART = 2,
        CB_CLOSE = 3,
    } ClientBehavior_t;

    static void showServerMessage(const std::string& message, const std::string& title, uint32_t behavior);
    static void handlerError(uint32_t rc, Json::Value & json, bool isBackground);
};




#endif /* defined(__LSocketErrorHandler__) */
