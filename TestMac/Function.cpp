//
//  Function.cpp
//  Test2
//
//  Created by xuyi on 15/11/30.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "Function.h"
#include <iostream>

FunctionClass::FunctionClass()
{
    
}

FunctionClass::~FunctionClass()
{
    
}

void FunctionClass::setFun()
{
    _fun_map.insert({"name", [](){
        std::cout<<"fun name"<<std::endl;
    }});
}

std::function<void()> FunctionClass::getFun(const std::string & funName)
{
    return _fun_map[funName.c_str()];
}