#ifndef mlib_http_h
#define mlib_http_h

#include "mlib_helpers.h"
#include "mlib_event.h"
#include "mlib_shared_queue.h"

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>

MLIB_NS_BEGIN

class MHttpResponse;

typedef std::unordered_map<std::string, std::string> MHttpHeaders;

class MHttpRequest : public MEventDispatcher
{
public:
    static const std::string EVENT_FINISHED;
    static const std::string EVENT_CANCELLED;
    static const std::string EVENT_DELETE;
    
    enum RequestMethod {
        GET,
        POST,
    };
    
    enum Priority {
        LOW = 0,
        NORMAL,
        HIGH
    };
    
    static MHttpRequest * Request(const std::string& url) { return new MHttpRequest(url); }
    virtual ~MHttpRequest();
    
    template<typename T>
    void addParameter(const std::string& paramName, const T& value) {
        std::stringstream ss; ss << value; this->addParameterValue(paramName, ss.str());
    }
    void resetParameters();
    void setHeader(const std::string& headerName, const std::string& value);
    
    virtual void send();
    void cancel();
    
    void onSuccess(std::function<void(MHttpRequest*)> handler);
    void onError(std::function<void(MHttpRequest*)> handler);

    MLIB_DECLARE_PROPERTY(Priority, priority)
    MLIB_DECLARE_PROPERTY(RequestMethod, method);
    MLIB_DECLARE_PROPERTY(std::string, url);
    MLIB_DECLARE_PROPERTY(long, timeoutInSeconds);
    MLIB_DECLARE_PROPERTY(long, networkTimeout);
    MLIB_DECLARE_READONLY_WEAK_PROPERTY(MHttpResponse *, response);
    MLIB_DECLARE_READONLY_PROPERTY(bool, isSuccess);
    MLIB_DECLARE_READONLY_PROPERTY(bool, isCancelled);
    
    MLIB_DECLARE_PROPERTY(float, prog);
    MLIB_DECLARE_PROPERTY(double, downloadTotal);
    MLIB_DECLARE_PROPERTY(double, downloadNow);

protected:
    MHttpRequest(const std::string& url); // we don't want user to get one on stack
    virtual MHttpResponse * createResponse(unsigned short statusCode, const char * data, size_t size, MHttpHeaders& headers);
    
    
private:
    
    std::stringstream _paramStream;
    MHttpHeaders _headers;
    
    time_t _startTime;
    bool _finished;
    
    std::function<void(MHttpRequest*)> _successHandler;
    std::function<void(MHttpRequest*)> _errorHandler;
    
    void addParameterValue(const std::string& paramName, const std::string& value);

    static void Delete(MHttpRequest * &req);
    
    friend void __request_thread_run(MSharedQueue<MHttpRequest *> & requests, bool);

};

class MHttpResponse
{
public:
    enum {
        ERROR_CURL = 0xffff,
    }; // simulated error http status code
    
    friend class MHttpRequest;
    
    MHttpResponse(unsigned short statusCode, const char * data, size_t size, MHttpHeaders & headers);
    MHttpResponse(const MHttpResponse&) = delete;
    virtual ~MHttpResponse() {}
    
    virtual bool isValid() { return true; }
    
    MLIB_DECLARE_PROPERTY(bool, isErrorHandled);
    
    MLIB_DECLARE_READONLY_PROPERTY(unsigned short, statusCode);
    MLIB_DECLARE_READONLY_PROPERTY(std::string, responseData);
    MLIB_DECLARE_READONLY_PROPERTY(MHttpHeaders, headers);
    
    
};

MLIB_NS_END


#endif
