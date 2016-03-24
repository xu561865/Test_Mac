//
//  ClassA.cpp
//  Test2
//
//  Created by xuyi on 15/11/25.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "ClassA.h"
#include "json.h"
#include <cstdio>

void Base::run(char *str)
{
    printf("Base's run \n");
}

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
    printf("ClassA's run \n");
}

Json::Value ClassA::translateJson(const std::string & str)
{
    Json::Reader reader;
    Json::Value value;
    reader.parse(str, value);
    
    return value;
}

void ClassA::print()
{
    printf("ClassA's print \n");
}

void ClassA::print(int i, ...)
{
    va_list params;
    va_start(params, i);
    
    while(1)
    {
        int j = va_arg(params, int);
        if(j)
        {
            printf("%d \n", j);
        }
        else
        {
            break;
        }
    }
    
    
    va_end(params);
}