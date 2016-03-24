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

class Base
{
public:
    virtual void print() = 0;
    void run(char *);
};

class ClassA : public Base
{
public:
    ClassA();
    virtual ~ClassA();
    
    operator const char* ();
    void run(char*);
    Json::Value translateJson(const std::string &);
    
    virtual void print() override;
    void print(int, ...);
    
private:
    char* _str;
};

#endif /* ClassA_hpp */
