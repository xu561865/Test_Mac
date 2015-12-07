//
//  Hash_C.h
//  Test2
//
//  Created by xuyi on 15/12/4.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef Hash_C_h
#define Hash_C_h

#include "uthash.h"

struct Hash_ST
{
    int _id;
    UT_hash_handle hh;
};

class Hash_C
{
public:
    Hash_C();
    virtual ~Hash_C();
    
    void makeHashTable();
    void showHashTable();
    
private:
    Hash_ST *_hashTable;
};

#endif /* Hash_C_h */
