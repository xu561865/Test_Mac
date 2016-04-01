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
    
    LHttpRequest * auth(std::string password, std::string email/* = ""*/,bool check/* = false*/);
    
protected:
    LHttpRequest * createRequest(const std::string& module, const std::string& action, bool isSigned = true, bool isBackground = false);
    void preLogicRequestHandler(mlib::MEvent * evt);
    void postLogicRequestHandler(mlib::MEvent * evt);
    

};



#endif // lmessagecenter_h
