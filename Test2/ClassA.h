//
//  ClassA.hpp
//  Test2
//
//  Created by xuyi on 15/11/25.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef ClassA_hpp
#define ClassA_hpp

#include <string>
#include "json.h"

class ClassA
{
public:
    ClassA();
    virtual ~ClassA();
    
    operator const char* ();
    void run(char*);
    Json::Value translateJson(const std::string &);
    
private:
    char* _str;
};

#endif /* ClassA_hpp */
