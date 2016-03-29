#ifndef mlib_helpers_h
#define mlib_helpers_h

#define M_GET_2ND_ARG(arg1, arg2, ...) arg2
#define M_GET_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define M_GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4


#ifdef __cplusplus

#define MLIB_NS mlib

#ifdef MLIB_NS
#define MLIB_NS_BEGIN namespace MLIB_NS {
#define MLIB_NS_END }
#else
#define MLIB_NS
#define MLIB_NS_BEGIN
#define MLIB_NS_END
#endif

#define __MLIB_DECLARE_PROPERTY(type, reftype, creftype, name, setter, getter, setterModifier) \
public:\
    creftype getter() const \
    { \
        return _##name; \
    } \
    reftype getter() \
    { \
        return _##name; \
    } \
setterModifier \
    reftype setter(creftype a##name) \
    { \
        _##name = a##name; \
        return _##name; \
    } \
protected:\
    type _##name;


#define MLIB_DECLARE_READONLY_PROPERTY(type, name) \
__MLIB_DECLARE_PROPERTY(type, const type &, const type&, name, name, name, protected:)

#define MLIB_DECLARE_PROPERTY(type, name) \
__MLIB_DECLARE_PROPERTY(type, type &, const type&, name, name, name, public:)

#define MLIB_DECLARE_READONLY_WEAK_PROPERTY(type, name) \
__MLIB_DECLARE_PROPERTY(type, type, type, name, name, name, protected:)

#define MLIB_DECLARE_WEAK_PROPERTY(type, name) \
__MLIB_DECLARE_PROPERTY(type, type, type, name, name, name, public:)


#define MLIB_DECLARE_PROPERTY_SETTER_TO_IMPL(type, name) \
public:\
    inline const type & name() const { return _##name; } /* getter */ \
    inline type & name() { return _##name; } /* getter */ \
public:\
    void name(type a##name);\
protected:\
    type _##name;


#define MLIB_DECLARE_ATOMIC_PROPERTY(type, name, mutex) \
public: \
    inline const type & name() const { std::lock_guard<decltype(mutex)> guard(mutex); return _##name; } /* getter */ \
    inline type & name() { std::lock_guard<decltype(mutex)> guard(mutex); return _##name; } /* getter */ \
    inline void name(type a##name) { std::lock_guard<decltype(mutex)> guard(mutex); _##name = a##name; } /* setter */ \
protected:\
    type _##name;


#define MLIB_DEFINE_EVENT(eventname) \
static const std::string eventname = #eventname;

#endif // __cplusplus


#endif // mlib_helpers_h