//
//  mlib_task.cpp
//  legend
//
//  Created by Minhao Zhang on 7/9/13.
//
//

#include "mlib_task.h"
#include "mlib.h"
#include <thread>

static const std::string EVENT_TASK_SUCCESS_HANDLER_TO_BE_CALLED = "task_handler_success";
static const std::string EVENT_TASK_ERROR_HANDLER_TO_BE_CALLED = "task_handler_error";

MLIB_NS_BEGIN

MTask::MTask() :
_timeout(0),
_isStarted(false),
_isFinished(false),
_isSuccessful(false),
_isTimedOut(false),
_isCancelled(false),
_mutex(),
_cv(),
_timeoutThread()
{
    this->addEventListener(EVENT_TASK_FINISHED, MLIB_EVENT_CALLBACK(&MTask::onTaskFinished));
    this->addEventListener(EVENT_TASK_SUCCEEDED, [this] (MEvent * evt) {
        this->dispatchEvent(EVENT_TASK_SUCCESS_HANDLER_TO_BE_CALLED);
    });
    this->addEventListener(EVENT_TASK_FAILED, [this] (MEvent * evt) {
        this->dispatchEvent(EVENT_TASK_ERROR_HANDLER_TO_BE_CALLED);
    });
}

MTask::~MTask()
{
    if (isStarted() && !isFinished())
    {
        cancel();
    }
    
    if (_timeoutThread.joinable())
    {
        M_DEBUG("JOINING TIMEOUT THREAD WHEN DESTRUCTING MTASK!");
        _timeoutThread.join();
    }
}

void MTask::start()
{
    std::lock_guard<decltype(_mutex)> guard(_mutex);
    M_ASSERT(_isStarted == false || _isFinished == true);
    _isStarted = true;
    _isFinished = false;
    _isSuccessful = false;
    _isTimedOut = false;
    _isCancelled = false;
    
    if (_timeout)
    {
        uint32_t theTimeout = _timeout;
        _timeoutThread =
        std::thread([this, theTimeout] () {
            std::unique_lock<decltype(_mutex)> lock(_mutex);
            auto started = std::chrono::steady_clock::now();
            auto end = started + std::chrono::milliseconds(theTimeout);
            if (_cv.wait_until(lock, end) == std::cv_status::timeout)
            {
                M_DEBUG("TASK timed out");
                _isTimedOut = true;
                lock.unlock();
                if (!isFinished())
                {
                    this->dispatchEvent(EVENT_TASK_TIMEOUT);
                    this->finish(false);
                }
            }
            else // normal finish
            {
                M_DEBUG("TASK ended without timeout");
                
                lock.unlock();
            }
            
            M_DEBUG("TASK timeout thread ends");
        });
    }
}

void MTask::cancel()
{
    {
        std::lock_guard<decltype(_mutex)> guard(_mutex);
        M_ASSERT(_isStarted && !_isFinished);
        _isCancelled = true;
    }
    this->dispatchEvent(EVENT_TASK_CANCELLED);
    this->finish(false);
}

void MTask::finish(bool isSuccessful)
{
    {
        std::lock_guard<decltype(_mutex)> guard(_mutex);
        if (_isFinished && _isTimedOut)
        {
            return;
        }
        M_ASSERT(_isStarted && !_isFinished)
        _isFinished = true;
        _isSuccessful = isSuccessful;
    }
    this->dispatchEvent(EVENT_TASK_FINISHED);
    _cv.notify_all();
    //if (_timeoutThread.joinable()) _timeoutThread.detach();
}

void MTask::onSuccess(MEventListenerFunction handler)
{
    {
        std::lock_guard<decltype(_mutex)> guard(_mutex);
        this->removeEventListenerFor(&_successHandler);
        _successHandler = handler;
        this->addEventListener(EVENT_TASK_SUCCESS_HANDLER_TO_BE_CALLED, [this] (MEvent * evt) {
            _successHandler(evt);
        }, &_successHandler);
    }
    
    if (isFinished())
    {
        this->dispatchEvent(EVENT_TASK_SUCCESS_HANDLER_TO_BE_CALLED);
    }
}

void MTask::onError(MEventListenerFunction handler)
{
    {
        std::lock_guard<decltype(_mutex)> guard(_mutex);
        this->removeEventListenerFor(&_errorHandler);
        _errorHandler = handler;
        this->addEventListener(EVENT_TASK_ERROR_HANDLER_TO_BE_CALLED, [this] (MEvent * evt) {
            _errorHandler(evt);
        }, &_errorHandler);
    }
    
    if (isFinished())
    {
        this->dispatchEvent(EVENT_TASK_ERROR_HANDLER_TO_BE_CALLED);
    }
}

void MTask::onTaskFinished(mlib::MEvent *evt)
{
    if (isSuccessful())
    {
        this->dispatchEvent(EVENT_TASK_SUCCEEDED);
    }
    else
    {
        this->dispatchEvent(EVENT_TASK_FAILED);
    }
}


MLIB_NS_END
