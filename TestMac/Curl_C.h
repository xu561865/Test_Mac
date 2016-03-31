//
//  Curl_C.h
//  TestMac
//
//  Created by 徐以 on 4/1/16.
//  Copyright © 2016 xuyi. All rights reserved.
//

#ifndef Curl_C_h
#define Curl_C_h


class Curl_C
{
public:
    static Curl_C* sharedInstance();
    
    void testUrl();
};

#endif /* Curl_C_h */
