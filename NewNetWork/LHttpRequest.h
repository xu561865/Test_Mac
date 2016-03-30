#ifndef lhttprequest_h
#define lhttprequest_h

#include "mlib.h"
#include "json.h"
#include "mlib_ccext.h"

class LHttpResponse;

class LHttpRequest : public mlib::MHttpRequest
{
public:
    static LHttpRequest * Request(const std::string& url) { return new LHttpRequest(url); }
    LHttpResponse * parsedResponse() const;
    
    virtual void send();
    
    MLIB_DECLARE_PROPERTY(bool, isBackground);
    
protected:
    LHttpRequest(const std::string& url);
    virtual mlib::MHttpResponse * createResponse(unsigned short statusCode, const char * data, size_t size, mlib::MHttpHeaders & headers);
    
//    cocos2d::CCNode *_loadingView;
};

class LHttpResponse : public mlib::MHttpResponse
{
public:

    LHttpResponse(unsigned short statusCode, const char * data, size_t size, mlib::MHttpHeaders & headers);
    LHttpResponse(const LHttpResponse&) = delete;
    ~LHttpResponse();
    
    virtual bool isValid();
    
    MLIB_DECLARE_READONLY_PROPERTY(unsigned short, returnCode);
    MLIB_DECLARE_READONLY_PROPERTY(Json::Value, jsonValue);
//    MLIB_DECLARE_RETAIN_PROPERTY(cocos2d::CCObject *, context);
};

#endif // lhttprequest_h
