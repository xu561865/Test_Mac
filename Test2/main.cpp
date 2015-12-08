//
//  main.cpp
//  Test2
//
//  Created by xuyi on 15/8/17.
//  Copyright (c) 2015年 xuyi. All rights reserved.
//

#include <iostream>
#include "ClassA.h"
#include <string>
#include "json.h"
#include "Function.h"
#include "Hash_C.h"
#include "MI_C.h"


int main(int argc, const char * argv[]) {    
#ifdef T1
    ClassA a;
    Json::Value val = a.translateJson(std::string("aabbcc"));
    Json::Value va;
    va = val;
#endif
    
#ifdef FUN1
    //test fun
    FunctionClass fun;
    fun.setFun();
    fun.getFun("name")();
#endif
    
#define HASHTEST
#ifdef HASHTEST
    //test hash
    Hash_C c;
    User *user = new User();
    c.addHashTable(user);
    c.showHashTable();
#endif

#ifdef MITEST
    A a;
    MI_C mic;
    mic.runFun(&a, fun_micro(C::funC));
#endif
    
    std::cout << "Hello, World!\n";
    return 0;
}


