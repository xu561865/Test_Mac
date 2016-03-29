#ifndef lmessagecenter_h
#define lmessagecenter_h

#include "mlib.h"

#include "LHttpRequest.h"

class LMessageCenter
{
public:
    static LMessageCenter * SharedInstance();
    
    MLIB_DECLARE_PROPERTY(std::string, api);
    
public:
    // account
    
    
protected:
    LHttpRequest * createRequest(const std::string& module, const std::string& action, bool isSigned = true, bool isBackground = false);
    void preLogicRequestHandler(mlib::MEvent * evt);
    void postLogicRequestHandler(mlib::MEvent * evt);
    

};

#define MSG_CENTER (LMessageCenter::SharedInstance())



#endif // lmessagecenter_h
