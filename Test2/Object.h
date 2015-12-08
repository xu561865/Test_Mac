//
//  Object.h
//  Test2
//
//  Created by xuyi on 15/12/8.
//  Copyright © 2015年 xuyi. All rights reserved.
//

#ifndef Object_h
#define Object_h

typedef unsigned int uint32_t;

class Object
{
public:
    Object();
    virtual ~Object();
    virtual uint32_t getID() = 0;
    virtual void setID(uint32_t) = 0;
    
protected:
    uint32_t _id;
};

class User : public Object
{
public:
    User();
    virtual ~User();
    virtual uint32_t getID() override;
    virtual void setID(uint32_t) override;
};

#endif /* Object_h */
