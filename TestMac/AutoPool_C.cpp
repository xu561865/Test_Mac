//
//  AutoPool_C.cpp
//  Test2
//
//  Created by xuyi on 15/12/10.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include <stdio.h>
#include "AutoPool_C.h"
#include "CCAutoreleasePool.h"

void AutoPool_C::ReleaseObject()
{
    CCObject *obj = new CCObject();
    obj->autorelease();
    
    CCPoolManager::sharedPoolManager()->pop();
}