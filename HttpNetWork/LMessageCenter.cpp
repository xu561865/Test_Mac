#include "LMessageCenter.h"
#include "LHttpErrorHandler.h"
//#include "LMemory.h"

#include <sstream>


LMessageCenter * LMessageCenter::SharedInstance()
{
    static LMessageCenter inst;
    return &inst;
}

LHttpRequest * LMessageCenter::createRequest(const std::string& module, const std::string& action, bool isSigned, bool isBackground)
{
    LHttpRequest * req = LHttpRequest::Request(_api);
    
    req->setHeader("M-Compress-Message", "1");
    
    req->method(LHttpRequest::POST);
    
    req->isBackground(isBackground);
    
    req->addEventListener(LHttpRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LMessageCenter::preLogicRequestHandler), this, INT_MIN); // highest priority
    req->addEventListener(LHttpRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LMessageCenter::postLogicRequestHandler), this, INT_MAX); // lowest priority
    
    return req;
}

void LMessageCenter::preLogicRequestHandler(mlib::MEvent *evt)
{
    auto req = dynamic_cast<const LHttpRequest *>(evt->target());

    auto json = req->parsedResponse()->jsonValue();
    
    //test
    if (0 == json["st"].asUInt())
    {
        M_DEBUG("st " << json["st"].asUInt() << ", today " <<  json["today"].asUInt() << ", time zone " <<  json["time_zone"].asUInt() << ", code " << req->parsedResponse()->returnCode());
    }
 
    if (req->parsedResponse()->returnCode() != LHttpErrorHandler::ERROR_NETWORK)
    {

    }
    
    if (req->parsedResponse()->returnCode() == LHttpErrorHandler::ERR_SERVER_MESSAGE)
    {
        auto msg = json["message"].asString(),
        title = json["title"].asString();
        auto behavior = json["behavior"].asUInt();
        
        LHttpErrorHandler::showServerMessage(msg, title, behavior);
        evt->cancel();
    }
    else if (req->parsedResponse()->returnCode() == LHttpErrorHandler::ERR_CLIENT_NEEDS_UPDATE)
    {
        auto version = json["version"].asString();
        
        evt->cancel();
    }
}


void LMessageCenter::postLogicRequestHandler(mlib::MEvent *evt)
{
    auto req = dynamic_cast<const LHttpRequest *>(evt->target());
    
    if (req->isSuccess())
    {
        
    }
    else 
    {
        M_ERROR("response error: " << req->parsedResponse()->responseData());
        if (!req->response()->isErrorHandled())
        {
            auto rc = req->parsedResponse()->returnCode();
            auto json = req->parsedResponse()->jsonValue();
            LHttpErrorHandler::handlerError(rc, json, req->isBackground());
        }
    }
}

LHttpRequest * LMessageCenter::auth(std::string password, std::string email/* = ""*/,bool check/* = false*/)
{
    auto req = this->createRequest("account", "auth", false);

    req->addParameter("pass", password);
    req->addParameter("check", check);
    
    
    auto handler = [req, password,check] (mlib::MEvent * evt) {
        if (req->isSuccess())
        {
        }
    };
    req->addEventListener(LHttpRequest::EVENT_FINISHED, handler);
    
    return req;
}
