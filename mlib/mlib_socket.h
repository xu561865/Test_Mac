#ifndef mlib_socket_h
#define mlib_socket_h

#include "mlib_helpers.h"
#include "mlib_event.h"
#include "mlib_shared_queue.h"

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "mlib_utils.h"

MLIB_NS_BEGIN

class MSocketResponse;


class MSocketRequest : public MEventDispatcher
{
public:
    virtual ~MSocketRequest();
    
    static const std::string EVENT_FINISHED;
    static const std::string EVENT_CANCELLED;
    static const std::string EVENT_DELETE;
    
    enum SOCKET_STATE
    {
        SOCKET_WAIT_CONNECT = 0,
        SOCKET_SUCCESS,
        SOCKET_CANCELLED,
        SOCKET_FAILED
    };
    
    enum Priority
    {
        LOW = 0,
        NORMAL,
        HIGH
    };
    
    static MSocketRequest * Request(const std::string& url)
    {
        return new MSocketRequest(url);
    }
    
    template<typename T>
    void addParameter(const std::string& paramName, const T& value)
    {
        std::stringstream ss;
        ss << value;
        _paramStream << "&" << paramName << "=" << urlencode(ss.str());
    }
    
    virtual void send();
    void cancel();
    
    void onSuccess(std::function<void(MSocketRequest*)> handler);
    void onError(std::function<void(MSocketRequest*)> handler);
    
    MLIB_DECLARE_PROPERTY(Priority, priority)
    MLIB_DECLARE_PROPERTY(SOCKET_STATE, socketState);
    MLIB_DECLARE_PROPERTY(std::string, url);
    MLIB_DECLARE_PROPERTY(long, timeoutInSeconds);
    MLIB_DECLARE_PROPERTY(long, networkTimeout);
    MLIB_DECLARE_READONLY_WEAK_PROPERTY(MSocketResponse *, response);
    MLIB_DECLARE_READONLY_PROPERTY(bool, isSuccess);
    MLIB_DECLARE_READONLY_PROPERTY(bool, isCancelled);
    
    
protected:
    MSocketRequest(const std::string& url); // we don't want user to get one on stack
    virtual MSocketResponse * createResponse(unsigned short statusCode, const char * data, size_t size);
    
    
private:
    
    std::stringstream _paramStream;
    size_t _paramLen;
    
    std::string _host;
    int _iport;
    int _hSocket;
    
    time_t _startTime;
    bool _finished;
    
    std::function<void(MSocketRequest*)> _successHandler;
    std::function<void(MSocketRequest*)> _errorHandler;
    
    static void Delete(MSocketRequest * &req);
    
    friend void __request_thread_run(MSharedQueue<MSocketRequest *> & requests, bool);
    
};

class MSocketResponse
{
public:
    enum
    {
        OK = 0,
        ERROR = 0xffff,
    }; // simulated error http status code
    
    friend class MSocketRequest;
    
    MSocketResponse(unsigned short statusCode, const char * data, size_t size);
    MSocketResponse(const MSocketResponse&) = delete;
    virtual ~MSocketResponse() {}
    
    virtual bool isValid() { return true; }
    
    MLIB_DECLARE_PROPERTY(bool, isErrorHandled);
    
    MLIB_DECLARE_READONLY_PROPERTY(unsigned short, statusCode);
    MLIB_DECLARE_READONLY_PROPERTY(std::string, responseData);
    
};

MLIB_NS_END


#endif
