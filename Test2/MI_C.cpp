//
//  MI_C.cpp
//  Test2
//
//  Created by xuyi on 15/12/7.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "MI_C.h"
#include <iostream>

A::A()
{
    
}

A::~A()
{
    
}

void A::funA(int i)
{
    std::cout<<"funA = "<<i<<std::endl;
}

B::B()
{
    
}

B::~B()
{
    
}

C::C()
{
    
}

C::~C()
{
    
}

void C::funC(int i)
{
    std::cout<<"funC = "<<i<<std::endl;
}

MI_C::MI_C()
{
    
}

MI_C::~MI_C()
{
    
}

void MI_C::runFun(A* pA, FUNA fun)
{
    (pA->*fun)(3);
}