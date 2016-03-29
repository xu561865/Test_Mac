//
//  LHttpErrorHandler.cpp
//  legend
//
//  Created by Minhao Zhang on 5/17/13.
//
//

#include "LHttpErrorHandler.h"

#include "MScreenManager.h"
#include "LMessageCenter.h"
#include "LAlertView.h"
#include "LMemory.h"
#include "IntroScene.h"
#include "LTextTooltipView.h"
#include "MainScene.h"
#include "LConfigSpec.h"
#include "cocos2d.h"

USING_NS_CC;

void LHttpErrorHandler::showServerMessage(const std::string& message, const std::string& title, uint32_t behavior)
{
    LAlertView::ShowAlertTextWithHighestPriority(
                          MLIB_LOCALIZED_STRING(message),
                          MLIB_LOCALIZED_STRING(title),
                          ALERT_CLOSE,
                          "",
                          [behavior] (std::string btnTag) {
                              switch (behavior) {
                                  case CB_SHUTDOWN:
                                      ((std::string *)nullptr)->length();
                                      break;
                                  case CB_RESTART:
                                      CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                                      break;
                                  case CB_CLOSE:
                                      // do nothing
                                      break;
                                      
                                  default:
                                      break;
                              }
                          }
                          );
}

void LHttpErrorHandler::handlerError(uint32_t rc, Json::Value &json, bool isBackground)
{
    mlib::runInMainThread([rc, json, isBackground] () {
        
        switch (rc) {
            case ERROR_PARSE_JSON:
            case ERROR_DEFLATE_DATA:
                if (!isBackground)
                {
                    LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("错误解析服务器返回文本。"),
                                                  [] (std::string tag) {
                                                      CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                                                  }
                                                  );

                }
                break;
                
            case ERROR_NETWORK:
                if (!isBackground)
                    LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("网络错误，请重试！"));
                break;
                
            case ERR_SPEC_EXPIRED:
                // reload spec
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("游戏数据有变更，请重新载入。"),
                                              [] (std::string tag) {
                                                  CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                                              }
                                              );
                break;
                
            case ERR_CLIENT_NEEDS_RESET:
                MEM_INFO->clear();
                MEM_INFO->saveBasic();
                break;
                
            case ERR_TOKEN_MISMATCH:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("你的账号在其他设备上登陆了。是否重新登陆？"),
                                              [] (std::string tag) {
                                                  CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                                              });
                break;
            case ERR_TOKEN_EXPIRED:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("您的登陆已过期, 请重新登陆"), [] (std::string tag) {
                    CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                });
                break;
                
            case ERR_ACCOUNT_NOT_EXIST:
            case ERR_ACCOUNT_AUTH_FAILED:
#ifdef DEBUG
                MEM_INFO->clear();
                MEM_INFO->reset();
                MEM_INFO->accountId() = 0;
                MEM_INFO->saveBasic();
#endif
                break;
                
#pragma mark Friend
            case ERR_FRIEND_ENERGY_LIMIT_EXCEEDED:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING_FORMAT_P1("您今天领取的体力已经达到上限。每天只能接受 %u 次好友赠送。", CONFIG->friendSendLimit()));
                break;
            case ERR_FRIEND_LIMIT_EXCEEDED:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("您已经达到好友数量上限，无法添加好友。"));
                break;
            case ERR_FRIEND_LIMIT_EXCEEDED_FOR_OTHER:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("对方好友数量已达上限，无法添加好友。"));
                break;
            case ERR_FRIEND_ALREADY_EXIST:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("这个玩家已经是你的好友了。"));
                break;
                
#pragma mark Guild
            case ERR_GUILD_APPLICATION_ALREADY_PROCESSED:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("申请已过期。"));
            {
                auto req = MSG_CENTER->getGuildApplications();
                req->isBackground() = true;
                req->send();
            }
                break;
            case ERR_GUILD_GUEST_CREATE:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("先起自己取个名字，再来创建忍者村吧。"));
                break;
            case ERR_GUILD_NAME_EXIST:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("该名称已经被占用。"));
                break;
            case ERR_GUILD_USER_LACK_OF_PERMISSION:
                LTextTooltipView::show(MLIB_LOCALIZED_STRING("糟糕！你不能执行这个操作。"));
                break;
            case ERR_GUILD_LEVEL_TOO_LOW:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("忍者村等级不足！"));
                break;
            case ERR_GUILD_USER_ALONE:
            {
                MEM_INFO->guild()->guildId(0);
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("很遗憾，你被踢出了忍者村。"), [](std::string tag)
                                                             {
                                                                 MainScene::switchTab(kMainSceneTabGuildEntry);
                                                             });
                
                break;
            }
                
#pragma mark Gem
            case ERR_GEM_MAX_LEVEL:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("符石已经达到最大等级。"));
                break;
                
#pragma mark Shop
            case ERR_SHOP_DAILY_CASH_DISABLED:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("每日金币奖励活动结束！"));
                break;
            case ERR_SHOP_DAILY_CASH_ALREADY_CLAIMED:
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING("你今天已经领取过了。"));
                break;
                
            default:
                M_ERROR("Error code not processed, code = " << rc);
                LAlertView::ShowAlertTextWithHighestPriority(MLIB_LOCALIZED_STRING_FORMAT_P1("发生未知错误，错误代码 = %u\n点击确定重新加载。",
                                                                              rc
                                                                              ),
                                              [] (std::string tag) {
                                                  CCDirector::sharedDirector()->replaceScene(IntroScene::scene());
                                              }
                                              );
                break;
        }
    });
}