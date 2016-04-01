

#include "LSocketErrorHandler.h"
#include "mlib.h"


void LSocketErrorHandler::showServerMessage(const std::string& message, const std::string& title, uint32_t behavior)
{

}

void LSocketErrorHandler::handlerError(uint32_t rc, Json::Value &json, bool isBackground)
{
    mlib::runInMainThread([rc, json, isBackground] () {
        
        switch (rc) {
            case ERROR_PARSE_JSON:
            case ERROR_DEFLATE_DATA:
                if (!isBackground)
                {
                    
                }
                break;
                
            case ERROR_NETWORK:
                if (!isBackground)
                break;
                
            case ERR_SPEC_EXPIRED:
                break;
                
            case ERR_CLIENT_NEEDS_RESET:
                
                break;
                
            case ERR_TOKEN_MISMATCH:
                
                break;
            case ERR_TOKEN_EXPIRED:
                
                break;
                
        }
    });
}