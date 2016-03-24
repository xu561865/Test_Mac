//
//  Function.h
//  Test2
//
//  Created by xuyi on 15/11/30.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef Function_h
#define Function_h

#include <map>
#include <functional>

class FunctionClass
{
public:
    FunctionClass();
    virtual ~FunctionClass();
    
    void setFun();
    std::function<void()> getFun(const std::string&);
    
private:
    std::map<std::string, std::function<void()>> _fun_map;
};

#endif /* Function_h */
