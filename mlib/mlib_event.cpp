#include "mlib_event.h"

#include "mlib_log.h"
#include "mlib_thread.h"

MLIB_NS_BEGIN

#pragma mark MEvent

MEvent::MEvent() :
_type(""),
_bubbles(true),
_shouldStopPropogation(false),
_shouldStopImmediatePropogation(false),
_target(nullptr),
_currentTarget(nullptr),
_context(nullptr),
_buffer(),
_isCancelled(false)
{
}


MEvent::MEvent(const std::string& type, void * context) :
MEvent()
{
    _type = type;
    _context = context;
}

MEvent::MEvent(const char *type, void * context) :
MEvent(std::string(type), context)
{
    
}

MEvent::MEvent(const MEvent& other) :
MEvent()
{
    _type = other._type;
    _bubbles = other._bubbles;
}

MEvent MEvent::clone()
{
    MEvent e(*this);
    return e;
}

void MEvent::cancel()
{
    _isCancelled = true;
}

void MEvent::stopPropogation()
{
    _shouldStopPropogation = true;
}

void MEvent::stopImmediatePropogation()
{
    _shouldStopImmediatePropogation = true;
}
#pragma mark MEventDispatcher

MEventDispatcher::MEventDispatcher() :
_parentDispatcher(nullptr),
_dispatchInMainThread(true)
{
    
}

void MEventDispatcher::addEventListener(const std::string &eventType, MEventListenerFunction listener, void * identifier, int priority)
{
    std::vector<MEventListener> & vec = _listeners[eventType];
    
    auto iter = vec.begin();
    while (iter != vec.end()) {
        if (iter->priority > priority)
        {
            break;
        }
        iter++;
    }
    MEventListener wrapper;
    wrapper.identifier = identifier;
    wrapper.priority = priority;
    wrapper.func = std::move(listener);
    
    vec.insert(iter, std::move(wrapper));
    
    return;
}

void MEventDispatcher::removeAllEventListeners()
{
    _listeners.clear();
}

void MEventDispatcher::removeEventListenerFor(void *identifier)
{
    for (auto & pair:_listeners)
    {
        auto & vec = pair.second;
        for (auto wrapper = vec.begin(); wrapper != vec.end(); )
        {
            if (wrapper->identifier == identifier)
            {
                wrapper = vec.erase(wrapper);
            }
            else
            {
                wrapper++;
            }
        }
    }
}

void MEventDispatcher::dispatchEvent(MEvent&& event)
{
    this->dispatchEvent(event);
}

void MEventDispatcher::dispatchEvent(MEvent& event)
{
    if (event._target == nullptr) event._target = this;
    event._currentTarget = this;
    
    std::vector<MEventListener> & vec = _listeners[event._type];
    
    for (auto listener: vec)
    {
        if (_dispatchInMainThread)
        {
            runInMainThread([&listener,&event](){
                listener.func(&event);
            });
        }
        else
        {
            listener.func(&event);
        }
        
        if (event.isCancelled()) return;
        
        if (event._shouldStopImmediatePropogation)
        {
            break;
        }
    }
    
    if (event._bubbles && !event._shouldStopPropogation)
    {
        if (_parentDispatcher)
        {
            _parentDispatcher->dispatchEvent(event);
        }
    }
}

void MEventDispatcher::dispatchEvent(const std::string &eventType)
{
    this->dispatchEvent(MEvent(eventType));
}

void MEventDispatcher::dispatchEvent(const char *eventType)
{
    this->dispatchEvent(MEvent(eventType));
}

MLIB_NS_END