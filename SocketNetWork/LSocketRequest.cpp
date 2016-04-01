#include "LSocketRequest.h"
//#include "LMemory.h"
#include "LMessageCenter.h"
#include "LSocketErrorHandler.h"

using namespace mlib;

#pragma mark LSocketRequest

LSocketRequest::LSocketRequest(const std::string& url) : MSocketRequest::MSocketRequest(url)
{
    _timeoutInSeconds = 5;
}

MSocketResponse * LSocketRequest::createResponse(unsigned short statusCode, const char * data, size_t size)
{
    runInMainThread([&](){
        if (!isBackground())
        {
//            MScreenManager::closePopup(_loadingView);
        }
    });
    
    LSocketResponse * resp = new LSocketResponse(statusCode, data, size);
    
    return resp;
}

LSocketResponse * LSocketRequest::parsedResponse() const
{
    return dynamic_cast<LSocketResponse *>(_response);
}

void LSocketRequest::send()
{
    if (isBackground())
    {
        priority(NORMAL);
    }
    else
    {
        priority(HIGH);
    }
    
    MSocketRequest::send();
    
    if (!isBackground())
    {
        M_DEBUG("loadingview popup");
    }
}

#pragma mark LSocketResponse

LSocketResponse::LSocketResponse(unsigned short statusCode, const char * data, size_t size) : MSocketResponse(statusCode, data, size)
{
    
    mlib::runInMainThread([this, statusCode] () {
        switch (statusCode) {
            case 200:
            case 301:
            case 302:
            {
                    Json::Reader reader;
                    if (reader.parse(this->responseData(), _jsonValue))
                    {
                        if (!_jsonValue.isObject() || !_jsonValue.isMember("rc"))
                        {
                            M_ERROR("response is not a valid json");
                            _returnCode = LSocketErrorHandler::ERROR_PARSE_JSON;
                        }
                        else
                        {
                            _returnCode = _jsonValue["rc"].asUInt();
                        }
                    }
                    else
                    {
                        _returnCode = LSocketErrorHandler::ERROR_PARSE_JSON;
                    }
                }
            
                break;
                
            default:
            {
                _returnCode = LSocketErrorHandler::ERROR_NETWORK;
            }
                break;
        }
    });
}

LSocketResponse::~LSocketResponse()
{
//    MLIB_PROPERTY_RELEASE(cocos2d::CCObject *, context);
}

bool LSocketResponse::isValid()
{
    return (_returnCode == 0);
}
