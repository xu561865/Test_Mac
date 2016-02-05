//
//  MI_C.h
//  Test2
//
//  Created by xuyi on 15/12/7.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef MI_C_h
#define MI_C_h

class A;
typedef void (A::*FUNA)(int);
#define fun_micro(fun) (FUNA)(&fun)

class A
{
public:
    A();
    virtual ~A();
    void funA(int);
};

class B
{
public:
    B();
    virtual ~B();
};

class C : public A, public B
{
public:
    C();
    virtual ~C();
    void funC(int);
};

class MI_C
{
public:
    MI_C();
    virtual ~MI_C();
    
    void runFun(A *pA, FUNA);
};

#endif /* MI_C_h */
