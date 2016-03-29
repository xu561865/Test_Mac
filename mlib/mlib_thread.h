#ifndef mlib_thread_h
#define mlib_thread_h

#include "mlib_helpers.h"
#include <functional>

MLIB_NS_BEGIN
void runInMainThread(std::function<void()> func);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void setMainThread(pthread_t tid);
#endif

MLIB_NS_END

#endif // mlib_thread_h
