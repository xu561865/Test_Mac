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
            delete(head->_obj);
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
//            hs->_id = rand() % TEST_MAX_LEN;
//            HASH_ADD_INT(_hashTable, _id, hs);
        }
    }
}

void Hash_C::showHashTable()
{
    for(Hash_ST* head = _hashTable; head != nullptr; head = static_cast<Hash_ST*>(head->hh.next))
    {
        std::cout<<"id = "<<head->_obj->getID()<<"\n";
    }
}

bool Hash_C::addHashTable(Object* obj)
{
    static uint32_t ID = 0;
    
    bool ret = false;
    if(!obj)
    {
        return ret;
    }
    
    Hash_ST *st = nullptr;
    HASH_FIND_INT(_hashTable, &obj, st);
    if(!st)
    {
        st = static_cast<Hash_ST*>(malloc(sizeof(Hash_ST)));
        st->_obj = obj;
        obj->setID(++ID);
        
        HASH_ADD_INT(_hashTable, _obj, st);
    }
    
    return ret;
}

bool Hash_C::delHashTable(Object *obj)
{
    bool ret = false;
    
    Hash_ST *st = nullptr;
    HASH_FIND_INT(_hashTable, &obj, st);
    if(st)
    {
        ret = true;
        delete st->_obj;
        HASH_DEL(_hashTable, st);
    }
    else
    {
        return ret;
    }
    
    return ret;
}
