#ifndef mlib_event_h
#define mlib_event_h

#include "mlib_helpers.h"
#include "mlib_buffer.h"

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <memory>

MLIB_NS_BEGIN

class MEventDispatcher;

class MEvent
{
public:
    friend class MEventDispatcher;
    
    MEvent();
    MEvent(const char *type, void * context = nullptr);
    MEvent(const std::string& type, void * context = nullptr);
    MEvent(const MEvent& other);
    MEvent & operator = (const MEvent& other) = delete;
    virtual ~MEvent() {}
    
    MLIB_DECLARE_READONLY_PROPERTY(std::string, type);
    MLIB_DECLARE_READONLY_PROPERTY(std::string, description);
    MLIB_DECLARE_READONLY_PROPERTY(bool, isCancelled);
    MLIB_DECLARE_READONLY_WEAK_PROPERTY(void *, context);
    MLIB_DECLARE_PROPERTY(MBuffer, buffer);

    MLIB_DECLARE_READONLY_WEAK_PROPERTY(MEventDispatcher *, target);
    MLIB_DECLARE_READONLY_WEAK_PROPERTY(MEventDispatcher *, currentTarget);
    
public:
    MEvent clone();
    
    void cancel();
    void stopPropogation();
    void stopImmediatePropogation();
    
protected:
    bool        _bubbles;
    bool        _shouldStopPropogation;
    bool        _shouldStopImmediatePropogation;
};

typedef std::function<void(MEvent*)> MEventListenerFunction;


class MEventDispatcher
{
    class MEventListener
    {
    public:
        void * identifier;
        MEventListenerFunction func;
        int priority;
    };
    
public:
    
    MEventDispatcher();
    virtual ~MEventDispatcher() {}

    void addEventListener(const std::string& eventType, MEventListenerFunction listener, void * identifier = nullptr, int priority = 0);
    void removeAllEventListeners();
    void removeEventListenerFor(void * identifier);
    void dispatchEvent(MEvent&& event);
    void dispatchEvent(MEvent& event);
    void dispatchEvent(const std::string& eventType);
    void dispatchEvent(const char * eventType);
    
    MLIB_DECLARE_WEAK_PROPERTY(MEventDispatcher *, parentDispatcher);
    MLIB_DECLARE_PROPERTY(MBuffer, context);
    MLIB_DECLARE_PROPERTY(bool, dispatchInMainThread);

protected:
private:
    std::map<std::string, std::vector<MEventListener> > _listeners;
};

MLIB_NS_END

#define EVENT_CHANGED "event_changed"

#define MLIB_DECLARE_EVENT_PROPERTY(type, name, eventType) \
public:\
inline type name() { return _##name; } /* getter */ \
inline void name(type const & a##name) { \
if (_##name != a##name) { \
_##name = a##name; \
this->dispatchEvent(MLIB::MEvent(eventType)); \
this->dispatchEvent(MLIB::MEvent(EVENT_CHANGED));\
} /* if */ \
} /* setter */ \
protected:\
type _##name;

#define MLIB_EVENT_CALLBACK(mfp) std::bind(mfp, this, std::placeholders::_1)

#endif // mlib_event_h
