#include "LMessageCenter.h"
#include "LMemory.h"


#include "LHttpErrorHandler.h"
#include "mlib_localize.h"
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
    req->addParameter("_spec", MEM_INFO->specSignature());
    
    if (req->url() == "")
    {
        req = req;
    }
    
    std::stringstream ss;
    
    ss << module << "." << action;
    req->addParameter("_msg", ss.str());
    
    if (isSigned)
    {
        ss.str("");
        ss << MEM_INFO->userId() << "." << MEM_INFO->token()
        << "." << MEM_INFO->tokenExpireAt();
        req->addParameter("_sig", ss.str());
        
        if (MEM_INFO->tokenExpireAt() < MEM_INFO->serverTime() + 3600) // expire in 1hour
        {
            if (module != "account")
            {
                auto re_sign = MSG_CENTER->auth(MEM_INFO->passwordHashed());
                re_sign->isBackground() = true;
                re_sign->send();
            }
        }
    }
    
    req->isBackground(isBackground);
    
    req->addEventListener(LHttpRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LMessageCenter::preLogicRequestHandler), this, INT_MIN); // highest priority
    req->addEventListener(LHttpRequest::EVENT_FINISHED, MLIB_EVENT_CALLBACK(&LMessageCenter::postLogicRequestHandler), this, INT_MAX); // lowest priority
    
    return req;
}

void LMessageCenter::preLogicRequestHandler(mlib::MEvent *evt)
{//10
    auto req = dynamic_cast<const LHttpRequest *>(evt->target());

    auto json = req->parsedResponse()->jsonValue();
    
    //test
    if (0 == json["st"].asUInt())
    {
        M_DEBUG("st " << json["st"].asUInt() << ", today " <<  json["today"].asUInt() << ", time zone " <<  json["time_zone"].asUInt() << ", code " << req->parsedResponse()->returnCode());
    }
 
    if (req->parsedResponse()->returnCode() != LHttpErrorHandler::ERROR_NETWORK)
    {
        MEM_INFO->updateServerTime(json["st"].asUInt());
        MEM_INFO->today(json["today"].asUInt());
        MEM_INFO->timeZoneOffset(json["time_zone"].asInt());
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
        
        LAlertView::ShowAlertTextWithHighestPriority(lutils::get_update_text(version), [](std::string btnTag) {
            lutils::update_client();
        });
        evt->cancel();
    }
    else if (json.isMember("update"))
    {
        auto jsonUpdate = json["update"];
//        M_DEBUG("\n" << req->parsedResponse()->responseData());
        
        for (auto k : jsonUpdate.getMemberNames())
        {
            M_INFO("Got server pushed notif '" << k << "'");
            
            if (k == "user")
            {
                MEM_INFO->hostProfile()->parseJson(jsonUpdate[k]);
            }
            else if (k == "daily_info")
            {
                auto di = LUserDailyInfo::createWithJson(jsonUpdate[k]);
                MEM_INFO->dailyInfo(di);
            }
            else if (k == "guild_info")
            {
                if (MEM_INFO->guildInfo())
                {
                    MEM_INFO->guildInfo()->parseJson(jsonUpdate[k]);
                }
                else
                {
                    MEM_INFO->guildInfo(LUserGuildInfo::createWithJson(jsonUpdate[k]));
                }
            }
            else if (k == "guild")
            {
                if (!jsonUpdate[k].isNull())
                {
                    if (MEM_INFO->guild())
                    {
                        MEM_INFO->guild()->parseJson(jsonUpdate[k]);
                    }
                    else
                    {
                        MEM_INFO->guild(LGuild::createWithJson(jsonUpdate[k]));
                    }
                }
                else
                {
                    MEM_INFO->guild(nullptr);
                }

            }
            else if (k == "guild_app")
            {
                auto arr = CCArray::create();
                for (auto i : jsonUpdate[k])
                {
                    auto app = LGuildApplication::createWithJson(i);
                    arr->addObject(app);
                }
                MEM_INFO->guildApplications(arr);
            }
            else if (k == "achievement")
            {
                auto achv = LUserAchievements::createWithJson(jsonUpdate[k]);
                MEM_INFO->achievements(achv);
            }
            else if (k == "battle_ability")
            {
                if (MEM_INFO->hostProfile())
                {
                    if( !MEM_INFO->isGrowthing() )
                        MEM_INFO->abilityMark(MEM_INFO->hostProfile()->ability());
                    
                    MEM_INFO->hostProfile()->ability() = jsonUpdate[k].asUInt();
                    
                    if( !MEM_INFO->isGrowthing() )
                        MEM_INFO->showAbility();
                    
                    MEM_INFO->dispatchEvent(EVENT_BAND_CHANGED);
                }
            }
            else if (k == "mail")
            {
                auto req = MSG_CENTER->getMails();
                req->isBackground(true);
                req->send();
            }
            else if (k == "f")
            {
                auto req = MSG_CENTER->getFriends();
                req->isBackground(true);
                req->send();
            }
            else if (k == "freq")
            {
                auto req = MSG_CENTER->getFriendRequests();
                req->isBackground(true);
                req->send();
            }
            else if (k == "rating")
            {
                auto req = MSG_CENTER->getCampaignRatings();
                req->isBackground(false);
                req->send();
            }
        }
    }
}


void LMessageCenter::postLogicRequestHandler(mlib::MEvent *evt)
{//31
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