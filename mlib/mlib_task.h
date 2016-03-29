//
//  mlib_task.h
//  legend
//
//  Created by Minhao Zhang on 7/9/13.
//
//

#ifndef __legend__mlib_task__
#define __legend__mlib_task__

#include <iostream>
#include "mlib_helpers.h"
#include "mlib_event.h"
#include <thread>
#include <mutex>
#include <condition_variable>

MLIB_NS_BEGIN

static const std::string EVENT_TASK_FINISHED = "event_task_finished";
static const std::string EVENT_TASK_SUCCEEDED = "event_task_succeeded";
static const std::string EVENT_TASK_FAILED = "event_task_failed";
static const std::string EVENT_TASK_CANCELLED = "event_task_cancelled";
static const std::string EVENT_TASK_TIMEOUT = "event_task_timeout";

class MTask : public MEventDispatcher
{
public:
    MTask();
    virtual ~MTask();
    
    virtual void start();
    virtual void cancel();
    virtual void finish(bool isSuccessful);

    void onSuccess(MEventListenerFunction handler);
    void onError(MEventListenerFunction handler);

    MLIB_DECLARE_ATOMIC_PROPERTY(uint32_t, timeout, _mutex);
    MLIB_DECLARE_ATOMIC_PROPERTY(bool, isStarted, _mutex);
    MLIB_DECLARE_ATOMIC_PROPERTY(bool, isFinished, _mutex);
    MLIB_DECLARE_ATOMIC_PROPERTY(bool, isSuccessful, _mutex);
    MLIB_DECLARE_ATOMIC_PROPERTY(bool, isTimedOut, _mutex);
    MLIB_DECLARE_ATOMIC_PROPERTY(bool, isCancelled, _mutex);
    
private:
    void onTaskFinished(MEvent * evt);
    MEventListenerFunction _successHandler;
    MEventListenerFunction _errorHandler;
    mutable std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _timeoutThread;
};

MLIB_NS_END


#endif /* defined(__legend__mlib_task__) */
