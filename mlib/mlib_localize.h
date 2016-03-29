//
//  mlib_localize.h
//  legend
//
//  Created by Xu Jinyang on 4/9/13.
//
//

#ifndef __legend__mlib_localize__
#define __legend__mlib_localize__

#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include "mlib_helpers.h"
#include "cocos2d.h"
#include "mlib_assert.h"

#define LOC_ERROR 0xffff

MLIB_NS_BEGIN

class MLocalizationManager
{
public:
    MLocalizationManager() {}
    virtual ~MLocalizationManager() {}
    
    static MLocalizationManager & sharedInstance();
    
    void loadFile(const std::string & filename);
    std::string operator[] (const std::string& key);
    std::string StringFormatParamName(const char* format,
                                      const char* p1name = "",
                                      const char* p2name = "",
                                      const char* p3name = "",
                                      const char* p4name = "");
    std::string ParamNameTransformString(const char* format,
                                         const char* p1name = "",
                                         const char* p2name = "",
                                         const char* p3name = "",
                                         const char* p4name = "");
    uint32_t SortParam(const char* format);
    
    template<typename... args>
    std::string getLocalizedFormatStringP1(std::tuple<args...> formatParams, std::string str, ...);
    
    template<typename... args>
    std::string getLocalizedFormatStringP2(std::tuple<args...> formatParams, std::string str, ...);
    
    template<typename... args>
    std::string getLocalizedFormatStringP3(std::tuple<args...> formatParams, std::string str, ...);
    
    template<typename... args>
    std::string getLocalizedFormatStringP4(std::tuple<args...> formatParams, std::string str, ...);
private:
    std::map<std::string, std::string> _dictionary;
    
private:
    
};

struct MFontDefinition
{
    std::string fontName;
    float       fontSize;
    
    bool operator == (const MFontDefinition & other)
    {
        return (fontName == other.fontName && fabsf(fontSize - other.fontSize) < 0.000001);
    }
};

template<typename... args>
std::string MLocalizationManager::getLocalizedFormatStringP1(std::tuple<args...> formatParams, std::string str, ...)
{
    int32_t paramNum = 1;
     
    std::vector<const char *> vecParamNames(4, "");
    
    va_list params;
    va_start(params, str);
    
    uint32_t i = 0;
    auto num = paramNum;
    while (num > 0)
    {
        auto arg = va_arg(params, const char *);
        if (arg == nullptr)
        {
            break;
        }
        
//        vecParamNames[i] = arg;
        vecParamNames[i] = ""; // 由于脚本问题, 暂屏蔽此功能
        i++;
        num--;
    }
    
    va_end(params);
    
    std::string vstrFormat = ParamNameTransformString(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    std::string strLoc = StringFormatParamName(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    auto paramSort = SortParam(vstrFormat.c_str());
    if (paramSort == LOC_ERROR)
    {
        M_ASSERT(0, "error lang: " << str);
        strLoc = str;
        paramSort = 0;  // 0  默认顺序
    }
    
    auto p1 = std::get<0>(formatParams);
    
    return cocos2d::CCString::createWithFormat(strLoc.c_str(), p1)->getCString();
}

template<typename... args>
std::string MLocalizationManager::getLocalizedFormatStringP2(std::tuple<args...> formatParams, std::string str, ...)
{
    int32_t paramNum = 2;
    
    std::vector<const char *> vecParamNames(4, "");
    
    va_list params;
    va_start(params, str);
    
    uint32_t i = 0;
    auto num = paramNum;
    while (num > 0)
    {
        auto arg = va_arg(params, const char *);
        if (arg == nullptr)
        {
            break;
        }
        
//        vecParamNames[i] = arg;
        vecParamNames[i] = ""; // 由于脚本问题, 暂屏蔽此功能
        i++;
        num--;
    }
    
    va_end(params);
    
    std::string vstrFormat = ParamNameTransformString(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    std::string strLoc = StringFormatParamName(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    auto paramSort = SortParam(vstrFormat.c_str());
    if (paramSort == LOC_ERROR)
    {
        M_ASSERT(0, "error lang: " << str);
        strLoc = str;
        paramSort = 1;  // 01  默认顺序
    }
    
    auto p1 = std::get<0>(formatParams);
    auto p2 = std::get<1>(formatParams);
    
    std::string ret;
    
    switch (paramSort) {
        case 1: // 01
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p2)->getCString();
            break;
        case 10:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p1)->getCString();
            break;
            
        default:
            M_ASSERT(0, "error lang: " << str);
            break;
    }
    
    return ret;
}

template<typename... args>
std::string MLocalizationManager::getLocalizedFormatStringP3(std::tuple<args...> formatParams, std::string str, ...)
{
    int32_t paramNum = 3;
    
    std::vector<const char *> vecParamNames(4, "");
    
    va_list params;
    va_start(params, str);
    
    uint32_t i = 0;
    auto num = paramNum;
    while (num > 0)
    {
        auto arg = va_arg(params, const char *);
        if (arg == nullptr)
        {
            break;
        }
        
//        vecParamNames[i] = arg;
        vecParamNames[i] = ""; // 由于脚本问题, 暂屏蔽此功能
        i++;
        num--;
    }
    
    va_end(params);
    
    std::string vstrFormat = ParamNameTransformString(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    std::string strLoc = StringFormatParamName(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    auto paramSort = SortParam(vstrFormat.c_str());
    if (paramSort == LOC_ERROR)
    {
        M_ASSERT(0, "error lang: " << str);
        strLoc = str;
        paramSort = 12;  // 012  默认顺序
    }
    
    auto p1 = std::get<0>(formatParams);
    auto p2 = std::get<1>(formatParams);
    auto p3 = std::get<2>(formatParams);
    
    std::string ret;
    
    switch (paramSort) {
        case 12: // 012
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p2, p3)->getCString();
            break;
        case 21:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p3, p2)->getCString();
            break;
        case 102:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p1, p3)->getCString();
            break;
        case 120:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p3, p1)->getCString();
            break;
        case 201:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p1, p2)->getCString();
            break;
        case 210:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p2, p1)->getCString();
            break;
            
        default:
            M_ASSERT(0, "error lang: " << str);
            break;
    }
    
    return ret;
}

template<typename... args>
std::string MLocalizationManager::getLocalizedFormatStringP4(std::tuple<args...> formatParams, std::string str, ...)
{
    int32_t paramNum = 4;
    
    std::vector<const char *> vecParamNames(4, "");
    
    va_list params;
    va_start(params, str);
    
    uint32_t i = 0;
    auto num = paramNum;
    while (num > 0)
    {
        auto arg = va_arg(params, const char *);
        if (arg == nullptr)
        {
            break;
        }
        
//        vecParamNames[i] = arg;
        vecParamNames[i] = ""; // 由于脚本问题, 暂屏蔽此功能
        i++;
        num--;
    }
    
    va_end(params);
    
    std::string vstrFormat = ParamNameTransformString(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    std::string strLoc = StringFormatParamName(str.c_str(), vecParamNames[0], vecParamNames[1], vecParamNames[2], vecParamNames[3]);
    
    auto paramSort = SortParam(vstrFormat.c_str());
    if (paramSort == LOC_ERROR)
    {
        M_ASSERT(0, "error lang: " << str);
        strLoc = str;
        paramSort = 123;  // 0123  默认顺序
    }
    
    auto p1 = std::get<0>(formatParams);
    auto p2 = std::get<1>(formatParams);
    auto p3 = std::get<2>(formatParams);
    auto p4 = std::get<3>(formatParams);
    
    std::string ret;
    
    switch (paramSort) {
        case 123: // 0123
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p2, p3, p4)->getCString();
            break;
        case 132:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p2, p4, p3)->getCString();
            break;
        case 213:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p3, p2, p4)->getCString();
            break;
        case 231:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p3, p4, p2)->getCString();
            break;
        case 312:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p4, p2, p3)->getCString();
            break;
        case 321:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p1, p4, p3, p2)->getCString();
            break;
            
        case 1023:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p1, p3, p4)->getCString();
            break;
        case 1032:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p1, p4, p3)->getCString();
            break;
        case 1203:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p3, p1, p4)->getCString();
            break;
        case 1230:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p3, p4, p1)->getCString();
            break;
        case 1302:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p4, p1, p3)->getCString();
            break;
        case 1320:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p2, p4, p3, p1)->getCString();
            break;
            
        case 2013:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p1, p2, p4)->getCString();
            break;
        case 2031:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p1, p4, p2)->getCString();
            break;
        case 2103:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p2, p1, p4)->getCString();
            break;
        case 2130:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p2, p4, p1)->getCString();
            break;
        case 2301:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p4, p1, p2)->getCString();
            break;
        case 2310:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p3, p4, p2, p1)->getCString();
            break;
            
        case 3012:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p1, p2, p3)->getCString();
            break;
        case 3021:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p1, p3, p2)->getCString();
            break;
        case 3102:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p2, p1, p3)->getCString();
            break;
        case 3120:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p2, p3, p1)->getCString();
            break;
        case 3201:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p3, p1, p2)->getCString();
            break;
        case 3210:
            ret = cocos2d::CCString::createWithFormat(strLoc.c_str(), p4, p3, p2, p1)->getCString();
            break;
            
        default:
            M_ASSERT(0, "error lang: " << str);
            break;
    }
    
    return ret;
}

MLIB_NS_END

#define MLIB_LOCALIZED_STRING(str) (((mlib::MLocalizationManager::sharedInstance())[str]).c_str())

#define MLIB_LOCALIZED_STRING_FORMAT_P1(str, param1, ...) \
(mlib::MLocalizationManager::sharedInstance().getLocalizedFormatStringP1(std::make_tuple(param1), str, ##__VA_ARGS__, nullptr).c_str())
#define MLIB_LOCALIZED_STRING_FORMAT_P2(str, param1, param2, ...) \
(mlib::MLocalizationManager::sharedInstance().getLocalizedFormatStringP2(std::make_tuple(param1, param2), str, ##__VA_ARGS__, nullptr).c_str())
#define MLIB_LOCALIZED_STRING_FORMAT_P3(str, param1, param2, param3, ...) \
(mlib::MLocalizationManager::sharedInstance().getLocalizedFormatStringP3(std::make_tuple(param1, param2, param3), str, ##__VA_ARGS__, nullptr).c_str())
#define MLIB_LOCALIZED_STRING_FORMAT_P4(str, param1, param2, param3, param4, ...) \
(mlib::MLocalizationManager::sharedInstance().getLocalizedFormatStringP4(std::make_tuple(param1, param2, param3, param4), str, ##__VA_ARGS__, nullptr).c_str())

#endif /* defined(__legend__mlib_localize__) */
