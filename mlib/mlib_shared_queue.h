#ifndef mlib_shared_queue_h
#define mlib_shared_queue_h

#include <thread>
#include <mutex>
#include <queue>
#include "mlib_helpers.h"
#include "mlib_log.h"
#include "mlib_assert.h"
#include <condition_variable>

MLIB_NS_BEGIN

template <class T>
class MSharedQueue
{
public:
    virtual ~MSharedQueue() {}

    size_t size() const { std::lock_guard<std::mutex> sentinel(_mutex); return _queue.size(); }
    bool empty() const { std::lock_guard<std::mutex> sentinel(_mutex); return _queue.empty(); }
    void clear() { std::lock_guard<std::mutex> sentinel(_mutex); while (_queue.size()) _queue.pop(); }
    
    void push(const T& data);
    bool pop(T& data, const std::chrono::milliseconds & timeout);
    void pop(T& data);

protected:
    std::queue<T>                   _queue;
    mutable std::mutex              _mutex;
    mutable std::condition_variable _condition;
    
};

template<typename T> void MSharedQueue<T>::push(const T& data)
{
    {
        std::lock_guard<std::mutex> sentinel(_mutex);
        _queue.push(data);
        M_ASSERT(_queue.size());
        
    }
//    M_DEBUG("notifying one in queue! size of queue = " << _queue.size());
    _condition.notify_one();
}

template<typename T> bool MSharedQueue<T>::pop(T& data, const std::chrono::milliseconds & timeout)
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_queue.empty())
    {
        _condition.wait_for(lock, timeout);
    }
    if (!_queue.empty())
    {
        data = _queue.front();
        _queue.pop();
        return true;
    }
    else
    {
        return false;
    }
}

template<typename T> void MSharedQueue<T>::pop(T& data)
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (_queue.empty())
    {
        _condition.wait(lock);
    }

    data = _queue.front();
    _queue.pop();
}

MLIB_NS_END

#endif // mlib_shared_queue_h
