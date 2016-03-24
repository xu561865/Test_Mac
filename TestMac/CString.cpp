//
//  CString.cpp
//  Test2
//
//  Created by xuyi on 15/12/18.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "CString_C.h"
#include "CString.h"
#include <cstring>

void CString_C::test_CString()
{
    char *pStr = new char[10];
    strncpy(pStr, "aabbccdd\n", 9);
    
    char *pTemp = pStr;
    *pTemp = *pStr;
    
    CString str(pStr);
    str.print();
}