//
//  mlib_version_number.h
//  legend
//
//  Created by xiaofuan on 13-4-15.
//
//

#ifndef __legend__mlib_version_compare__
#define __legend__mlib_version_compare__

#include "mlib_helpers.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

MLIB_NS_BEGIN

class MVersionNumber
{
    // An internal utility structure just used to make the std::copy in the constructor easy to write.
    struct VersionDigit
    {
        double value;
        operator double() const
        {
            return value;
        }
    };
    
    friend std::istream& operator>>(std::istream& str, MVersionNumber::VersionDigit& digit);
public:
    
    MVersionNumber(std::string const& versionStr)
    {
        std::stringstream  versionStream(std::string(".") + versionStr);
        std::copy(std::istream_iterator<VersionDigit>(versionStream),
                  std::istream_iterator<VersionDigit>(),
                  std::back_inserter(versionInfo)
                  );
    }
    
    //compare
    bool operator<(MVersionNumber const& rhs) const
    {
        return std::lexicographical_compare(versionInfo.begin(), versionInfo.end(), rhs.versionInfo.begin(), rhs.versionInfo.end());
    }
    
private:
    std::vector<double>   versionInfo;
};

MLIB_NS_END

#endif /* defined(__legend__mlib_version_compare__) */
