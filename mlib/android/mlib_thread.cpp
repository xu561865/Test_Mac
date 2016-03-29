
//public final void runOnUiThread(Runnable action) {
//    if (Thread.currentThread() != mUiThread) {
//        mHandler.post(action);
//    } else {
//        action.run();
//    }
//}

//m2.runOnUiThread(new Runnable() {
//    public void run() {
//        Toast.makeText(m2,url,Toast.LENGTH_LONG).show();
//    }
//});
//}


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "mlib_thread.h"
#include "mlib_log.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"

MLIB_NS_BEGIN

static pthread_t g_mainThread = 0;

extern "C"
{
    void Java_org_cocos2dx_lo_legend_funcRun(JNIEnv*  env, jobject thiz,jint address)
    {
        std::function<void()> &func = *((std::function<void()> *)address);
        func();
    }
}


void runInMainThread(std::function<void()> func)
{
    if (pthread_self() == g_mainThread)
    {
        func();
    }
    else
    {
        cocos2d::JniMethodInfo minfo;
        bool isHave = cocos2d::JniHelper::getStaticMethodInfo(minfo, "org/cocos2dx/lo/legend", "runInMainThread", "(I)V");
        if (!isHave)
        {
            M_ERROR("jni:method not exist");
        }
        else
        {
//            M_INFO("jni:ok");
            minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, &func);
        }

        cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
    }
}

void setMainThread(pthread_t tid)
{
    g_mainThread = tid;
}

MLIB_NS_END

#endif

