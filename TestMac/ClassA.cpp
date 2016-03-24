//
//  ClassA.cpp
//  Test2
//
//  Created by xuyi on 15/11/25.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "ClassA.h"
#include "json.h"

ClassA::ClassA()
{
    
}

ClassA::~ClassA()
{
    
}

ClassA::operator const char *()
{
    return _str;
}

void ClassA::run(char * p)
{
    
}

Json::Value ClassA::translateJson(const std::string & str)
{
    Json::Reader reader;
    Json::Value value;
    reader.parse(str, value);
    
    return value;
}