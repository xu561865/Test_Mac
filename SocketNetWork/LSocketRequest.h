#ifndef lsocketrequest_h
#define lsocketrequest_h

#include "mlib_socket.h"
#include "json.h"
#include "mlib_ccext.h"

class LSocketResponse : public mlib::MSocketResponse
{
public:
    
    LSocketResponse(unsigned short statusCode, const char * data, size_t size);
    LSocketResponse(const LSocketResponse&) = delete;
    ~LSocketResponse();
    
    virtual bool isValid();
    
    MLIB_DECLARE_READONLY_PROPERTY(unsigned short, returnCode);
    MLIB_DECLARE_READONLY_PROPERTY(Json::Value, jsonValue);
};


class LSocketRequest : public mlib::MSocketRequest
{
public:
    static LSocketRequest * Request(const std::string& url) { return new LSocketRequest(url); }
    LSocketResponse * parsedResponse() const;
    
    virtual void send();
    
    MLIB_DECLARE_PROPERTY(bool, isBackground);
    
protected:
    LSocketRequest(const std::string& url);
    virtual mlib::MSocketResponse * createResponse(unsigned short statusCode, const char * data, size_t size);
    
};


#endif // lhttprequest_h
