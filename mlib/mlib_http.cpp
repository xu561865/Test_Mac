#include "mlib_http.h"

#include <curl/curl.h>

#include "mlib_log.h"
#include "mlib_utils.h"
#include "mlib_thread.h"
#include "mlib_buffer.h"

MLIB_NS_BEGIN

static std::recursive_mutex g_mutex;

const std::string MHttpRequest::EVENT_FINISHED = "event_finished";
const std::string MHttpRequest::EVENT_CANCELLED = "event_cancelled";
const std::string MHttpRequest::EVENT_DELETE = "event_delete";

static MSharedQueue<MHttpRequest *> g_requests_low;
static MSharedQueue<MHttpRequest *> g_requests_normal;
static MSharedQueue<MHttpRequest *> g_requests_high;

static uint32_t g_num_of_threads_low = 0;
static uint32_t g_num_of_threads_normal = 0;
static uint32_t g_num_of_threads_high = 0;

static size_t __write_curl_data(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto buf = static_cast<MBuffer *>(userdata);
    buf->appendData((const char *)ptr, size * nmemb);
    
    return size * nmemb;
}

static size_t __write_curl_header(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    auto pHeaders = static_cast<MHttpHeaders *>(userdata);
    
    std::string line((const char *)ptr, size * nmemb);
    auto pos = line.find(':');
    if (pos == std::string::npos)
    {
        
    }
    else
    {
        auto name = line.substr(0, pos);
        auto content = line.substr(pos + 1);
        
        name = trim(name);
        content = trim(content);
        
        pHeaders->insert(std::make_pair(name, content));
    }
    
    return size * nmemb;
}

static size_t __progress_func(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow)
{
    auto *req = (MHttpRequest *)ptr;
    req->downloadNow(dlnow);
    req->downloadTotal(dltotal);
    
    if (dltotal > 0)
    {
        req->prog(dlnow / dltotal);
    }
    else
    {
        req->prog(0);
    }
    
    return 0;
}

void __request_thread_run(MSharedQueue<MHttpRequest *> & requests, bool isTemp = false)
{
    
    CURL * ch = nullptr;
    MHttpRequest * req = nullptr;
    while (1)
    {
        if (requests.empty() && isTemp)
        {
            break;
        }
        
        requests.pop(req);

        if (req->_isCancelled)
        {
            MHttpRequest::Delete(req);
            continue;
        }
        
        auto now = time(NULL);
        if (now - req->_startTime > req->_timeoutInSeconds)
        {
            MHttpHeaders emptyHeaders;
            req->_response = req->createResponse(MHttpResponse::ERROR_CURL, nullptr, 0, emptyHeaders);
            req->_isSuccess = false;
        }
        else
        {
            if (ch)
            {
                curl_easy_reset(ch);
            }
            else
            {
                ch = curl_easy_init();
            }
            
            req->_isSuccess = false;
            CURLcode res;
            MBuffer buffer;
            MHttpHeaders respHeaders;
            struct curl_slist * slist = NULL;
            
            std::string real_url = req->_url;
            if (req->_method == MHttpRequest::GET)
            {
                auto tokenPos = real_url.find('?');
                if (tokenPos == std::string::npos)
                {
                    real_url += "?";
                }
                real_url += req->_paramStream.str();
            }
            
            res = curl_easy_setopt(ch, CURLOPT_URL, real_url.c_str());
            if (req->_method == MHttpRequest::POST)
            {
                res = curl_easy_setopt(ch, CURLOPT_POST, 1);
                if (req->_paramStream.str().size())
                {
                    res = curl_easy_setopt(ch, CURLOPT_COPYPOSTFIELDS, req->_paramStream.str().c_str());
                }
            }
            
            if (req->_headers.size() > 0)
            {
                for (auto i:req->_headers)
                {
                    std::string line = i.first + ": " + i.second;
                    slist = curl_slist_append(slist, line.c_str());
                }
                res = curl_easy_setopt(ch, CURLOPT_HTTPHEADER, slist);
            }
            
            res = curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
            res = curl_easy_setopt(ch, CURLOPT_TIMEOUT, req->_networkTimeout);
            res = curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, __write_curl_data);
            res = curl_easy_setopt(ch, CURLOPT_WRITEDATA, &buffer);
            res = curl_easy_setopt(ch, CURLOPT_HEADERFUNCTION, __write_curl_header);
            res = curl_easy_setopt(ch, CURLOPT_WRITEHEADER, &respHeaders);
            res = curl_easy_setopt(ch, CURLOPT_NOSIGNAL, 1);
            res = curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 0);
            res = curl_easy_setopt(ch, CURLOPT_PROGRESSFUNCTION, __progress_func);
            res = curl_easy_setopt(ch, CURLOPT_PROGRESSDATA, req);
            
            res = curl_easy_perform(ch);
            if (res == CURLE_OK)
            {
                req->_isSuccess = true;
            }
            else
            {
                req->_isSuccess = false;
                M_INFO("CURL returns error " << curl_easy_strerror(res) << ", url = " << req->_url << ", params = " << req->_paramStream.str());
            }
            
            if (slist != NULL) curl_slist_free_all(slist);
            
            if (req->_isSuccess)
            {
                long code = 0;
                res = curl_easy_getinfo(ch, CURLINFO_HTTP_CODE, &code);
                
                {
                    req->_response = req->createResponse(code, buffer.getData(), buffer.size(), respHeaders);
                }
                
                req->_isSuccess = req->_isSuccess && req->_response->isValid();
            }
            else
            {
                req->_response = req->createResponse(MHttpResponse::ERROR_CURL, nullptr, 0, respHeaders);
            }
        }
        
        runInMainThread([req] () {
        
            if (!req->_isCancelled)
            {
                req->addEventListener(MHttpRequest::EVENT_FINISHED, [req] (mlib::MEvent * evt) {
                    if (req->_isSuccess)
                    {
                        if (req->_successHandler)
                        {
                            req->_successHandler(req);
                        }
                    }
                    else
                    {
                        if (req->_errorHandler)
                        {
                            req->_errorHandler(req);
                        }
                    }
                }, &g_mutex);
                
                req->dispatchEvent(MEvent(MHttpRequest::EVENT_FINISHED));
                req->removeEventListenerFor(&g_mutex);
            }
        });
        
        MHttpRequest::Delete(req);
    }
    
    if (ch) curl_easy_cleanup(ch);
}

static void __create_thread(MSharedQueue<MHttpRequest *> & queue, uint32_t & counter, uint32_t limit = 0, size_t count = 1)
{
    for (size_t i = 0; i < count; ++i)
    {
        std::thread([&queue, &counter, limit]() {
            {
                std::lock_guard<decltype(g_mutex)> guard(g_mutex);
                if (limit > 0 && counter >= limit)
                {
                    return;
                }
                counter ++;
            }
            __request_thread_run(queue, limit != 0);
            
            {
                std::lock_guard<decltype(g_mutex)> guard(g_mutex);
                if (counter > 0)
                {
                    counter--;
                }
            }
        }).detach();
    }
}



#pragma mark MHttpRequest

MHttpRequest::MHttpRequest(const std::string& url) :
_url(url),
_timeoutInSeconds(30),
_networkTimeout(30),
_method(MHttpRequest::GET),
_response(nullptr),
_startTime(0),
_finished(false),
_isSuccess(false),
_isCancelled(false),
_priority(NORMAL),
_prog(0),
_downloadTotal(0),
_downloadNow(0)
{
}

MHttpRequest::~MHttpRequest()
{
    if (_response)
    {
        delete _response; _response = nullptr;
    }
}

void MHttpRequest::setHeader(const std::string &headerName, const std::string &value)
{
    _headers[headerName] = value;
}

void MHttpRequest::addParameterValue(const std::string &paramName, const std::string &value)
{
    _paramStream << "&" << paramName << "=" << urlencode(value);
}

void MHttpRequest::resetParameters()
{
    _paramStream.str("");
}

void MHttpRequest::cancel()
{
    runInMainThread([this] () {
        if (!_isCancelled)
        {
            M_TRACE("cancel request, url = " << _url);
            _isCancelled = true;
            dispatchEvent(MEvent(EVENT_CANCELLED));
        }
        else
        {
            M_WARNING("Request is already cancelled!");
        }
        
    });
}

void MHttpRequest::send()
{
    std::lock_guard<decltype(g_mutex)> guard(g_mutex);
    
    static bool threadInitialized = false;
    
    if (!threadInitialized)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        threadInitialized = true;
        __create_thread(g_requests_low, g_num_of_threads_low, 0 ,1);
        __create_thread(g_requests_normal, g_num_of_threads_normal, 0, 2);
        __create_thread(g_requests_high, g_num_of_threads_high, 0, 3);
        
        // monitor thread
        std::thread([](){

            while (1) {
                __create_thread(g_requests_low, g_num_of_threads_low, 2, g_requests_low.size());
                __create_thread(g_requests_normal, g_num_of_threads_normal, 10, g_requests_normal.size());
                __create_thread(g_requests_high, g_num_of_threads_high, 20, g_requests_high.size());
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
            
        }).detach();
    }
    
    _startTime = time(NULL);
    
    switch (_priority) {
        case LOW:
            g_requests_low.push(this);
            break;
        case NORMAL:
            g_requests_normal.push(this);
            break;
        case HIGH:
            g_requests_high.push(this);
            break;
            
        default:
            break;
    }
}

void MHttpRequest::Delete(mlib::MHttpRequest *&req)
{
    if (!req->_finished)
    {
        req->dispatchEvent(MEvent(EVENT_DELETE));
        req->_finished = true;
        delete req;

        req = nullptr;

    }
}

void MHttpRequest::onSuccess(std::function<void (MHttpRequest *)> handler)
{
    _successHandler = handler;
}

void MHttpRequest::onError(std::function<void (MHttpRequest *)> handler)
{
    _errorHandler = handler;
}

MHttpResponse * MHttpRequest::createResponse(unsigned short statusCode, const char *data, size_t size, MHttpHeaders &headers)
{
    return new MHttpResponse(statusCode, data, size, headers);
}

MHttpResponse::MHttpResponse(unsigned short statusCode, const char * data, size_t size, MHttpHeaders & headers) :
_statusCode(statusCode),
_responseData(data, size),
_headers(std::move(headers)),
_isErrorHandled(false)
{
    
}

#pragma mark MHttpResponse

MLIB_NS_END
