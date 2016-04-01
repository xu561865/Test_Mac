#include "LHttpRequest.h"
//#include "LMemory.h"
#include "LMessageCenter.h"
#include "LHttpErrorHandler.h"

using namespace mlib;

#pragma mark LHttpRequest

LHttpRequest::LHttpRequest(const std::string& url) : MHttpRequest::MHttpRequest(url)
{
    _timeoutInSeconds = 5;
}

MHttpResponse * LHttpRequest::createResponse(unsigned short statusCode, const char * data, size_t size, mlib::MHttpHeaders & headers)
{
    runInMainThread([&](){
        if (!isBackground())
        {
//            MScreenManager::closePopup(_loadingView);
        }
    });
    
    LHttpResponse * resp = new LHttpResponse(statusCode, data, size, headers);
    
    return resp;
}

LHttpResponse * LHttpRequest::parsedResponse() const
{
    return dynamic_cast<LHttpResponse *>(_response);
}

void LHttpRequest::send()
{
    if (isBackground())
    {
        priority(NORMAL);
    }
    else
    {
        priority(HIGH);
    }
    
    MHttpRequest::send();
    
    if (!isBackground())
    {
        M_DEBUG("loadingview popup");
//        LLoadingView *view = LLoadingView::createWithCCB();
//        MScreenManager::popupWithBgColor(view, cocos2d::ccc4(0, 0, 0, 0), true);
//        _loadingView = view;
    }
}

#pragma mark LHttpResponse

LHttpResponse::LHttpResponse(unsigned short statusCode, const char * data, size_t size, mlib::MHttpHeaders & headers) :
MHttpResponse(statusCode, data, size, headers)
{
//    MLIB_PROPERTY_INIT(cocos2d::CCObject *, context);
    
    mlib::runInMainThread([this, statusCode] () {
        switch (statusCode) {
            case 200:
            case 301:
            case 302:
            {
                //            M_DEBUG(std::string(data, size));
                bool dataOK = true;
                //            for (auto iter:_headers)
                //            {
                //                M_DEBUG("header: " << iter.first << ":" << iter.second);
                //            }
                if (_headers.find("M-Message-Compressed") != _headers.end()
                    && _headers["M-Message-Compressed"] == "1"
                    )
                {
                    std::string decoded = mlib::base64_decode(_responseData);
                    unsigned char * deflated = nullptr;
                    int len; //= cocos2d::ZipUtils::ccInflateMemory((unsigned char *)decoded.data(), decoded.length(), &deflated);
                    if (deflated == nullptr)
                    {
                        _returnCode = LHttpErrorHandler::ERROR_DEFLATE_DATA;
                    }
                    else
                    {
                        _responseData = std::string((const char *)deflated, len);
                        delete [] deflated;
                    }
                    
                    //                M_DEBUG("deflating json response: " << (dataOK ? "ok" : "error"));
                }
                
                if (dataOK)
                {
                    Json::Reader reader;
                    if (reader.parse(this->responseData(), _jsonValue))
                    {
                        if (!_jsonValue.isObject() || !_jsonValue.isMember("rc"))
                        {
                            M_ERROR("response is not a valid json");
                            _returnCode = LHttpErrorHandler::ERROR_PARSE_JSON;
                        }
                        else
                        {
                            _returnCode = _jsonValue["rc"].asUInt();
                        }
                    }
                    else
                    {
                        _returnCode = LHttpErrorHandler::ERROR_PARSE_JSON;
                    }
                }
            }
                break;
                
            default:
            {
                _returnCode = LHttpErrorHandler::ERROR_NETWORK;
            }
                break;
        }
    });
}

LHttpResponse::~LHttpResponse()
{
//    MLIB_PROPERTY_RELEASE(cocos2d::CCObject *, context);
}

bool LHttpResponse::isValid()
{
    return (_returnCode == 0);
}
