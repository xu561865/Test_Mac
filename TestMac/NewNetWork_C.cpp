//
//  NewNetWork_C.cpp
//  TestMac
//
//  Created by 徐以 on 3/30/16.
//  Copyright © 2016 xuyi. All rights reserved.
//

#include "LMessageCenter.h"
#include "NewNetWork_C.h"


void NewNetWork_C::testNewtNetWork()
{
    LHttpRequest *authReq = LMessageCenter::SharedInstance()->auth("", "", false);
    authReq->onSuccess([this](mlib::MHttpRequest *r) {
        M_DEBUG("login success");
        
        
    });
    authReq->onError([this](mlib::MHttpRequest *r) {
        
    });
    authReq->isBackground() = true;
    authReq->send();

}