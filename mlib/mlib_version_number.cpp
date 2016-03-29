//
//  mlib_version_compare.cpp
//  legend
//
//  Created by xiaofuan on 13-4-15.
//
//

#include "mlib_version_number.h"
#include <vector>

MLIB_NS_BEGIN

std::istream& operator>>(std::istream& str, MVersionNumber::VersionDigit& digit)
{
    str.get();
    str >> digit.value;
    return str;
}

MLIB_NS_END
