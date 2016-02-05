//
//  CString.h
//  Test2
//
//  Created by xuyi on 15/12/18.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef CString_h
#define CString_h

#include <cstring>

class CString
{
public:
    CString(char*);
    virtual ~CString();
    CString(const CString&);
    CString& operator=(const CString&);
    
    void print();
    
private:
    char *_str;
};

#endif /* CString_h */
