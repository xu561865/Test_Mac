//
//  CString.cpp
//  Test2
//
//  Created by xuyi on 15/12/18.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "CString.h"
//#include <stdlib.h>

CString::CString(char *str)
{
    _str = str;
}

CString::~CString()
{
    delete []_str;
}

CString::CString(const CString& src)
{
    
}

CString& CString::operator=(const CString & src)
{
    return *this;
}

void CString::print()
{
    //printf("%s", _str);
}