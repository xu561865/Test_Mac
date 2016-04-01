//
//  LHttpErrorHandler.h
//  legend
//
//  Created by Minhao Zhang on 5/17/13.
//
//

#ifndef __legend__LHttpErrorHandler__
#define __legend__LHttpErrorHandler__

#include <iostream>
#include "json.h"

class LHttpErrorHandler
{
public:
    
    typedef enum {
        // client error
        ERROR_PARSE_JSON = 60001,
        ERROR_DEFLATE_DATA = 60002,
        ERROR_NETWORK = 60003,
        
        // Reserved Constants
        ERR_SERVER_INTERNAL = 50001,
        ERR_MOD_NOT_FOUND = 50002,
        ERR_ACTION_NOT_SUPPORTED = 50003,
        ERR_MSG_NOT_SIGNED = 50004,
        ERR_TOKEN_EXPIRED = 50005,
        ERR_TOKEN_INVALID = 50006,
        ERR_SPEC_NOT_FOUND = 50007,
        ERR_SPEC_ERROR = 50008,
        ERR_REQ_ERROR = 50009,
        ERR_CLIENT_NEEDS_RESET = 50010, // nuclear weapon!!! be careful
        ERR_SPEC_EXPIRED = 50011,
        ERR_IAP_VERFICATION_FAILED = 50012,
        ERR_TOKEN_MISMATCH = 50013,
        ERR_SERVER_MESSAGE = 50014,
        ERR_CLIENT_NEEDS_UPDATE = 50015,
        
        // Account Related
        ERR_ACCOUNT_NOT_EXIST = 10001,
        ERR_ACCOUNT_ALREADY_EXIST = 10002,
        ERR_ACCOUNT_AUTH_FAILED = 10003,
        ERR_ACCOUNT_EMAIL_EXIST = 10004,
        ERR_ACCOUNT_ALREADY_REGISTERED = 10005,
        ERR_ACCOUNT_INFO_NOT_EXIST = 10006,
        ERR_ACCOUNT_SINA_NOT_EXIST = 10007,
        ERR_ACCOUNT_SINA_EXIST = 10008,
        ERR_ACCOUNT_QQ_NOT_EXIST = 10009,
        ERR_ACCOUNT_QQ_EXIST = 10010,
        ERR_ACCOUNT_FACEBOOK_NOT_EXIST = 10011,
        ERR_ACCOUNT_FACEBOOK_EXIST = 10012,
        
        // record not found
        ERR_USER_NOT_EXIST = 20001,
        ERR_CARD_NOT_EXIST = 20002,
        ERR_PROP_NOT_EXIST = 20003,
        ERR_DAILY_INFO_NOT_EXIST = 20004,
        ERR_FORMATION_NOT_EXIST = 20005,
        ERR_ACHIEVEMENT_NOT_EXIST = 20006,
        ERR_FRIENDS_INFO_NOT_EXIST = 20007,
        ERR_GUILD_NOT_EXIST = 20008,
        ERR_GUILD_INFO_NOT_EXIST = 20009,
        ERR_GUILD_APPLICATION_NOT_EXIST = 20010,
        
        // not enough resource
        ERR_NOT_ENOUGH_COIN = 20101,
        ERR_NOT_ENOUGH_CASH = 20102,
        ERR_NOT_ENOUGH_ENERGY = 20103,
        ERR_NOT_ENOUGH_SPACE = 20104,
        ERR_NOT_ENOUGH_PROP = 20105,
        ERR_NOT_ENOUGH_BADGE = 20106,
        ERR_NOT_ENOUGH_WEALTH = 20107,
        ERR_NOT_ENOUGH_CONTRIBUTION = 20108,
        
        // shop
        ERR_SHOP_ITEM_NOT_FOR_SALE = 20201,
        ERR_SHOP_DAILY_CASH_ALREADY_CLAIMED = 20202,
        ERR_SHOP_DAILY_CASH_DISABLED = 20203,
        
        // level too low
        ERR_LEVEL_NOT_MET_FOR_USER = 20301,
        ERR_LEVEL_NOT_MET_FOR_CARD = 20302,
        
        // inventory
        ERR_USE_LOCKED_CARD = 20401,
        ERR_MAX_BAG_REACHED = 20402,
        ERR_USE_UNCONSUMABLE_CARD = 20403,
        
        // formation
        ERR_FORMATION_SUPPLY_OVERFLOW = 20501,
        ERR_FORMATION_DUPLICATE_CARD = 20502,
        
        // campaign
        ERR_CAMPAIGN_STAGE_ALREADY_FINISHED = 20601,
        ERR_CAMPAIGN_STAGE_NOT_QUALIFIED = 20602,
        ERR_CAMPAIGN_STAGE_NOT_FINISHED = 20603,
        ERR_CAMPAIGN_USER_LEVEL_TOO_LOW = 20604,
        ERR_CAMPAIGN_DAILY_LIMIT_EXCEEDED = 20605,
        ERR_CAMPAIGN_VIP_EXPIRE = 20606,
        
        // card growth
        ERR_EVOLVE_NOT_MAX_LEVEL = 20701,
        ERR_EVOLVE_NOT_ALLOWED = 20702,
        ERR_EVOLVE_INVALID_MATERIAL = 20703,
        ERR_EVOLVE_MATERIAL_NOT_ENOUGH = 20704,
        
        // gem
        ERR_GEM_NOT_ENOUGH_TO_FUSE = 20801,
        ERR_GEM_MAX_LEVEL = 20802,
        ERR_GEM_NOT_EMPTY = 20803,
        ERR_GEM_NO_SLOT = 20804,
        ERR_GEM_NOT_FOR_UNMOUNT = 20805,
        ERR_GEM_SLOT_FULL = 20806,
        
        // equip
        ERR_EQUIP_NOT_TAKEN_OFF = 20901,
        ERR_EQUIP_LEVEL_NOT_MET = 20902,
        ERR_EQUIP_PROFESSION_NOT_MET = 20903,
        
        // quest
        ERR_QUEST_ALREADY_COMPLETED = 21001,
        ERR_QUEST_EXPECTATION_NOT_MET = 21002,
        
        // friends
        ERR_FRIEND_NOT_REQUESTED = 21101,
        ERR_FRIEND_NOT_EXIST = 21102,
        ERR_FRIEND_NOT_IN_CANDIDATE_LIST = 21103,
        ERR_FRIEND_NO_ENERGY_RECEIVED = 21104,
        ERR_FRIEND_ENERGY_LIMIT_EXCEEDED = 21105,
        ERR_FRIEND_LIMIT_EXCEEDED = 21106,
        ERR_FRIEND_LIMIT_EXCEEDED_FOR_OTHER = 21107,
        ERR_FRIEND_ALREADY_EXIST = 21108,
        
        // profile
        ERR_PROFILE_ALREADY_HAS_NAME = 21201,
        ERR_PROFILE_DUPLICATE_NAME = 21202,
        
        // mail
        ERR_MAIL_ATTACHMENT_CLAIMED = 21301,
        ERR_MAIL_ATTACHMENT_NOT_EXIST = 21302,
        ERR_MAIL_SENDER_HAS_NO_NAME = 21303,
        
        // arena
        ERR_ARENA_RANK_NOT_EXIST = 21401,
        ERR_ARENA_CHALLENGE_SELF = 21402,
        ERR_ARENA_CHALLENGE_EXCEEDS_LIMIT = 21403,
        
        // guild
        ERR_GUILD_APPLICATION_ALREADY_PROCESSED = 21501,
        ERR_GUILD_USER_ALREADY_JOINED = 21502,
        ERR_GUILD_USER_LACK_OF_PERMISSION = 21503,
        ERR_GUILD_FULL = 21504,
        ERR_GUILD_USER_ALONE = 21505,
        ERR_GUILD_USER_NOT_IN_GUILD = 21506,
        ERR_GUILD_NAME_EXIST = 21507,
        ERR_GUILD_LEVEL_TOO_LOW = 21508,
        ERR_GUILD_GUEST_CREATE = 21509,
        
        // newbie
        ERR_NEWBIE_ALREADY_BUY_ITEM = 21601,
        ERR_NEWBIE_ALREADY_ENHANCE = 21602,
        ERR_NEWBIE_ALREADY_EVOLVE = 21603,
        ERR_NEWBIE_ALREADY_SPIRIT_FUSE = 21604,
        
        //  world boss
        ERR_WORLDBOSS_FIGHT_NOT_ENOUGH = 21901,
        ERR_WORLDBOSS_ALREADY_KILLED = 21902,
        ERR_WORLDBOSS_NO_VALID_TIME = 21903,
        ERR_WORLDBOSS_ALREADY_ESCAPE = 21904,
        
        // cross server pvp
        ERR_PVP_SIGNED_UP_HAS_REACHED_MAX = 22704,
        
    } Errno_t;
    
    typedef enum {
        CB_SHUTDOWN = 1,
        CB_RESTART = 2,
        CB_CLOSE = 3,
    } ClientBehavior_t;

    static void showServerMessage(const std::string& message, const std::string& title, uint32_t behavior);
    static void handlerError(uint32_t rc, Json::Value & json, bool isBackground);
};




#endif /* defined(__legend__LHttpErrorHandler__) */
