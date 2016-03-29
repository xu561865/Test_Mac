#ifndef mlib_assert_h
#define mlib_assert_h

#include "mlib_helpers.h"
#include "mlib_exception.h"


MLIB_NS_BEGIN

MLIB_NS_END

#ifdef DEBUG

#define __M_ASSERT_NO_MSG(cond) \
if (!(cond)) { M_THROW("ASSERTION FAILED " << #cond << ", " << __FILE__ << "[" << __LINE__ << "]"); }

#define __M_ASSERT_WITH_MSG(cond, msg) \
{\
if (!(cond)) {\
M_THROW("ASSERTION FAILED " << #cond << ", " << msg << ", " << __FILE__ << "[" << __LINE__ << "]"); \
}\
}

#define __M_ASSERT_MACRO_CHOOSER(...) \
M_GET_3RD_ARG(__VA_ARGS__, __M_ASSERT_WITH_MSG, __M_ASSERT_NO_MSG)


#define M_ASSERT(...) __M_ASSERT_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#else
#define M_ASSERT(...)
#endif

#endif // mlib_assert_h