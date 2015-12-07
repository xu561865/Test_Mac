//
//  Hash_C.cpp
//  Test2
//
//  Created by xuyi on 15/12/4.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#include "Hash_C.h"
#include <iostream>

static uint32_t TEST_MAX_LEN = 10;

Hash_C::Hash_C()
{
    _hashTable = nullptr;
}

Hash_C::~Hash_C()
{
    if(_hashTable)
    {
        for(Hash_ST *head = _hashTable; head != nullptr; head = static_cast<Hash_ST*>(head->hh.next))
        {
            free(head);
        }
    }
    
    _hashTable = nullptr;
}

void Hash_C::makeHashTable()
{
    Hash_ST *hs;
    
    for(unsigned int i = 0; i < TEST_MAX_LEN; i++)
    {
        hs = static_cast<Hash_ST*>(malloc(sizeof(Hash_ST)));
        if(hs)
        {
            hs->_id = rand() % TEST_MAX_LEN;
            HASH_ADD_INT(_hashTable, _id, hs);
        }
    }
}

void Hash_C::showHashTable()
{
    for(Hash_ST* head = _hashTable; head != nullptr; head = static_cast<Hash_ST*>(head->hh.next))
    {
        std::cout<<head->_id<<"  ";
    }
}
