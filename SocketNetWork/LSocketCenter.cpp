#include "LSocketCenter.h"
#include "LSocketErrorHandler.h"
//#include "LMemory.h"

#include <sstream>


LSocketCenter * LSocketCenter::SharedInstance()
{
    static LSocketCenter inst;
    return &inst;
}

LSocketRequest * LSocketCenter::createRequest(const std::string& module, const std::string& action, bool isSigned, bool isBackground)
{
    LSocketRequest * req = LSocketRequest::Request(_api);
    
    req->isBackground(isBackground);
    
    req->addEventListener(LSocketRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LSocketCenter::preLogicRequestHandler), this, INT_MIN); // highest priority
    req->addEventListener(LSocketRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LSocketCenter::postLogicRequestHandler), this, INT_MAX); // lowest priority
    
    return req;
}

void LSocketCenter::preLogicRequestHandler(mlib::MEvent *evt)
{
    auto req = dynamic_cast<const LSocketRequest *>(evt->target());

    auto json = req->parsedResponse()->jsonValue();
    
    //test
    if (0 == json["st"].asUInt())
    {
        M_DEBUG("st " << json["st"].asUInt() << ", today " <<  json["today"].asUInt() << ", time zone " <<  json["time_zone"].asUInt() << ", code " << req->parsedResponse()->returnCode());
    }
 
    if (req->parsedResponse()->returnCode() != LSocketErrorHandler::ERROR_NETWORK)
    {

    }
    
    if (req->parsedResponse()->returnCode() == LSocketErrorHandler::ERR_SERVER_MESSAGE)
    {
        auto msg = json["message"].asString(),
        title = json["title"].asString();
        auto behavior = json["behavior"].asUInt();
        
        LSocketErrorHandler::showServerMessage(msg, title, behavior);
        evt->cancel();
    }
    else if (req->parsedResponse()->returnCode() == LSocketErrorHandler::ERR_CLIENT_NEEDS_UPDATE)
    {
        auto version = json["version"].asString();
        
        evt->cancel();
    }
}


void LSocketCenter::postLogicRequestHandler(mlib::MEvent *evt)
{
    auto req = dynamic_cast<const LSocketRequest *>(evt->target());
    
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
            LSocketErrorHandler::handlerError(rc, json, req->isBackground());
        }
    }
}

LSocketRequest * LSocketCenter::auth(std::string password, std::string email/* = ""*/,bool check/* = false*/)
{
    auto req = this->createRequest("account", "auth", false);

    req->addParameter("pass", password);
    req->addParameter("check", check);
    
    
    auto handler = [req, password,check] (mlib::MEvent * evt) {
        if (req->isSuccess())
        {
        }
    };
    req->addEventListener(LSocketRequest::EVENT_FINISHED, handler);
    
    return req;
}
