#include "mlib_thread.h"
#import "dispatch/queue.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <objc/runtime.h>
#import <Foundation/NSThread.h>

@interface MAutoFunc : NSObject

- (id)initWithFunc:(std::function<void()>) f;
- (void)run;
@end

@implementation MAutoFunc
{
    std::function<void()> _func;
}
- (id)initWithFunc:(std::function<void ()>)f
{
    if (self = [super init])
    {
        _func = f;
    }
    return self;
}

- (void)run
{
    _func();
}
@end

MLIB_NS_BEGIN

void runInMainThread(std::function<void()> func)
{
    if ([[NSThread currentThread] isMainThread])
    {
        func();
    }
    else
    {
        MAutoFunc * autofunc = [[MAutoFunc alloc] initWithFunc:func];
        dispatch_sync(dispatch_get_main_queue(), ^{
            [autofunc run];
        });
        [autofunc release];
    }
}

MLIB_NS_END
