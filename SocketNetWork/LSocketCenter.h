#ifndef lsocketcenter_h
#define lsocketcenter_h

#include "mlib.h"

#include "LSocketRequest.h"

class LSocketCenter
{
public:
    static LSocketCenter * SharedInstance();
    
    MLIB_DECLARE_PROPERTY(std::string, api);
    
public:
    
    LSocketRequest * auth(std::string password, std::string email/* = ""*/,bool check/* = false*/);
    
protected:
    LSocketRequest * createRequest(const std::string& module, const std::string& action, bool isSigned = true, bool isBackground = false);
    void preLogicRequestHandler(mlib::MEvent * evt);
    void postLogicRequestHandler(mlib::MEvent * evt);
    

};



#endif // lsocketcenter_h
