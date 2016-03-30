//
//  mlib_ccext_helpers.h
//  legend
//
//  Created by Xu Jinyang on 6/28/13.
//
//

#ifndef legend_mlib_ccext_helpers_h
#define legend_mlib_ccext_helpers_h

#define MLIB_CREATE_CCOBJECT(__TYPE__) \
static __TYPE__* create() { \
__TYPE__ * pRet = new __TYPE__();\
if (pRet) pRet->autorelease();\
return pRet;}

#define MLIB_IMPL_RETAIN_PROPERTY_SETTER(name) \
if (_##name != a##name) {\
CC_SAFE_RETAIN(a##name); \
CC_SAFE_RELEASE(_##name); \
_##name = a##name;}

#define MLIB_DECLARE_RETAIN_PROPERTY(type, name) \
public:\
inline type name() { return _##name; } /* getter */ \
public: \
inline void name(type a##name) { \
MLIB_IMPL_RETAIN_PROPERTY_SETTER(name); \
} /* setter */ \
protected:\
type _##name;

#define MLIB_DECLARE_RETAIN_PROPERTY_SETTER_TO_IMPL(type, name) \
public:\
inline type name() { return _##name; } /* getter */ \
void name(type a##name); \
protected:\
type _##name;

#define MLIB_DECLARE_CCBLOADER(className, superClassName) \
class className##Loader : public cocos2d::extension::superClassName##Loader{\
public:\
CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(className##Loader, loader);\
CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(className);\
};

#define MLIB_REGISTER_CCBLOADER(className) \
class __RegLoader##className {\
public: __RegLoader##className() {cocos2d::extension::CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary()->registerCCNodeLoader(#className, className##Loader::loader());}\
};\
static __RegLoader##className __reg__##className;

#define MLIB_CCB_MEMBER_GLUE_WEAK(type, name) \
CCB_MEMBERVARIABLEASSIGNER_GLUE_WEAK(this, #name, type, _##name)

#define MLIB_PROPERTY_INIT(type, name) \
_##name = nullptr

#define MLIB_PROPERTY_RELEASE(type, name) \
name(nullptr)

#define MLIB_CREATE_WITH_CCB(className, file) \
static className * createWithCCB(cocos2d::CCSize parentSize = cocos2d::CCDirector::sharedDirector()->getWinSize()) { \
cocos2d::extension::CCBReader *ccbReader = new cocos2d::extension::CCBReader(cocos2d::extension::CCNodeLoaderLibrary::sharedCCNodeLoaderLibrary()); \
ccbReader->autorelease(); \
return (className *)ccbReader->readNodeGraphFromFile(file, nullptr, parentSize); }

#define MLIB_IS_KIND_OF_CLASS(pObj, className) \
(typeid(*pObj) == typeid(className))

#define IMPLEMENTS_CCB_NODE \
public cocos2d::extension::CCBSelectorResolver, \
public cocos2d::extension::CCBMemberVariableAssigner, \
public cocos2d::extension::CCNodeLoaderListener

#endif
