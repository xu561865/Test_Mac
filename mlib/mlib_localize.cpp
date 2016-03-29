//
//  mlib_localize.cpp
//  legend
//
//  Created by Xu Jinyang on 4/9/13.
//
//

#include "mlib_localize.h"
#include "json.h"
#include "platform/CCFileUtils.h"
#include "mlib_utils.h"
#include "mlib_log.h"

#include <fstream>

MLIB_NS_BEGIN

using namespace std;
using namespace cocos2d;

MLocalizationManager & MLocalizationManager::sharedInstance()
{
    static MLocalizationManager man;
    return man;
}

void MLocalizationManager::loadFile(const std::string& filename)
{
    // don't clear because we might be loading multiple files
//    _dictionary.clear();
    
    unsigned long size = 0;
    unsigned char *data = CCFileUtils::sharedFileUtils()->getFileData(filename.c_str(), "r", &size);
    
    if (data != nullptr)
    {
        ifstream is(filename.c_str());
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char *)data, (const char *)data+size, root);
        
        auto keys = root.getMemberNames();
        for (auto k : keys)
        {
            _dictionary[k] = root[k].asString();
        }
        
        delete [] data;
    }
}

std::string MLocalizationManager::operator[](const std::string &key)
{
    if (_dictionary.find(key) == _dictionary.end()) return key;
    else return _dictionary[key];
}

std::string MLocalizationManager::StringFormatParamName( const char* format,
                                                         const char* p1name,
                                                         const char* p2name,
                                                         const char* p3name,
                                                         const char* p4name)

{
    std::string strFormat = format;
    std::size_t at = 0;
    std::vector<std::string> vtempPName;
    vtempPName.clear();
    int paramNum = 0;
    std::map<std::string, std::string> _paramType;
    _paramType.clear();
    while ((at = strFormat.find("%",at)) != std::string::npos)
    {
        if (at>=strFormat.size()) {
            break;
        }
        if ('%' == strFormat.at(++at)) {
            ++at;
            continue;
        }
        paramNum++;
    }
    if (1 == paramNum) {
        vtempPName.push_back(p1name);
    }
    if (2 == paramNum )
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
    }
    else if (3 == paramNum)
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
        vtempPName.push_back(p3name);
    }
    else if (4 == paramNum)
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
        vtempPName.push_back(p3name);
        vtempPName.push_back(p4name);
    }
    at = 0;
    for (int i = 0; i < paramNum; ++i)
    {
        at = strFormat.find("%",at);
        if (at>=strFormat.size()) {
            break;
        }
        char atChar = strFormat.at(at+1);
        if ('%' != atChar)
        {
            std::string temp = "";
            if (0 == strcmp("", vtempPName[i].c_str()))
            {
                temp+= "***p";
                temp+=SSTR(i+1);
                temp+="***";
            }
            else
            {
                temp = vtempPName[i];
            }
            size_t tempAt = at;
            char tempAtValue;
            while ((tempAtValue = strFormat.at(++tempAt))!= std::string::npos) {
                if ((short)tempAtValue > 65 && (short)tempAtValue < 122) {
                    break;
                }
            }
            size_t tempSize = tempAt - at + 1;
            tempSize = tempSize < 2 ? 2 : tempSize;
            std::string tempType = strFormat.substr(at,tempSize);
            _paramType.insert(make_pair(temp, tempType));
            strFormat.replace(at,tempSize,temp);
            at+=strlen(temp.c_str());
        }
        else
        {
            at+=2;
        }
    }
    std::string strValue = this->operator[](strFormat);
//    M_DEBUG(strValue);
    for (int i = 0; i < paramNum; ++i)
    {
        std::string temp = "";
        if (0 == strcmp("", vtempPName[i].c_str()))
        {
            temp+= "***p";
            temp+=SSTR(i+1);
            temp+="***";
        }
        else
        {
            temp = vtempPName[i];
        }
        
        string::size_type at1 = strValue.find(temp,0);
        if (at1 == string::npos)
        {
            M_ASSERT(0, "error lang: " << strValue);
            continue;
        }
        strValue.replace(at1,strlen(temp.c_str()),_paramType[temp]);
    }
    return strValue;
}

std::string MLocalizationManager::ParamNameTransformString(const char* format,
                                                           const char* p1name,
                                                           const char* p2name,
                                                           const char* p3name,
                                                           const char* p4name)
{
    std::string strFormat = format;
    std::size_t at = 0;
    std::vector<std::string> vtempPName;
    vtempPName.clear();
    int paramNum = 0;
    //std::map<std::string, std::string> _paramType;
    //_paramType.clear();
    while ((at = strFormat.find("%",at)) != std::string::npos)
    {
        if (at>=strFormat.size()) {
            break;
        }
        if ('%' == strFormat.at(++at)) {
            ++at;
            continue;
        }
        paramNum++;
    }
    
    if (1 == paramNum)
    {
        vtempPName.push_back(p1name);
    }
    else if (2 == paramNum )
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
    }
    else if (3 == paramNum)
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
        vtempPName.push_back(p3name);
    }
    else if (4 == paramNum)
    {
        vtempPName.push_back(p1name);
        vtempPName.push_back(p2name);
        vtempPName.push_back(p3name);
        vtempPName.push_back(p4name);
    }
    at = 0;
    for (int i = 0; i < paramNum; ++i)
    {
        at = strFormat.find("%",at);
        char atChar = strFormat.at(at+1);
        if ('%' != atChar)
        {
            std::string temp = "";
            if (0 == strcmp("", vtempPName[i].c_str()))
            {
                temp += "***p";
                temp += SSTR(i+1);
                temp += "***";
            }
            else
            {
                temp = vtempPName[i];
            }
            size_t tempAt = at;
            char tempAtValue;
            //找到%后第一个英文字母
            while ((tempAtValue = strFormat.at(++tempAt))!= std::string::npos) {
                //判断tempAtValue是A-Z或a-z之间的字母
                if (((short)tempAtValue > 65 && (short)tempAtValue < 90) || ((short)tempAtValue > 97 && (short)tempAtValue < 122)) {
                    break;
                }
            }
            size_t tempSize = tempAt - at + 1;
            tempSize = tempSize < 2 ? 2 : tempSize;
            strFormat.replace(at,tempSize,temp);
            at+=strlen(temp.c_str());
        }
        else
        {
            at+=1;
        }
    }
    return strFormat;
}

uint32_t MLocalizationManager::SortParam(const char* format)
{
    uint32_t result = 0;
    std::string strValue = "";
    std::string strFormat = format;
    std::string::size_type at = 0;
    int paramNum = 0;
    std::vector<std::string> vtempPName;
    
    strValue = this->operator[](format);
    
    while ((at = strFormat.find("***",at)) != std::string::npos)
    {
        at+=3;
        auto tempAt = strFormat.find("***",at);
        if (tempAt == std::string::npos)
        {
            // error data
            return LOC_ERROR;
        }
        std::string tempParamName = strFormat.substr(at, tempAt-at);
        vtempPName.push_back(tempParamName);
        at = tempAt+3;
        paramNum++;
    }
    
    
    int32_t check = 0;
    at = 0;
    while ((at = strValue.find("***",at)) != std::string::npos)
    {
        at+=3;
        auto tempAt = strValue.find("***",at);
        if (tempAt == std::string::npos)
        {
            // error data
            return LOC_ERROR;
        }
        
        std::string tempParamName = strValue.substr(at, tempAt-at);
        for (int i = 0; i < vtempPName.size(); ++i)
        {
            if (vtempPName[i] == tempParamName)
            {
                check += i;
                result = result * 10 + i;
                break;
            }
        }
        at = tempAt+3;
    }
    
    if (check != (0 + paramNum - 1) * paramNum / 2)
    {
        // error data
        return LOC_ERROR;
    }
    
//    M_DEBUG("Sort param result: " << result);
    return result;
}

MLIB_NS_END
