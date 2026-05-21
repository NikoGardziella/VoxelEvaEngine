#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

#ifdef VE_DEBUG

#define VE_INTERNAL_ASSERT_NO_MSG(check) \
    do \
    { \
        if (!(check)) \
        { \
            ENGINE_ERROR("Assertion '{0}' failed at {1}:{2}", VE_STRINGIFY_MACRO(check), __FILE__, __LINE__); \
            VE_DEBUGBREAK(); \
        } \
    } while (false)

#define VE_INTERNAL_ASSERT_WITH_MSG(check, ...) \
    do \
    { \
        if (!(check)) \
        { \
            ENGINE_ERROR(__VA_ARGS__); \
            VE_DEBUGBREAK(); \
        } \
    } while (false)

#define VE_INTERNAL_ASSERT_GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define ENGINE_ASSERT(...) \
    VE_EXPAND_MACRO(VE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, VE_INTERNAL_ASSERT_WITH_MSG, VE_INTERNAL_ASSERT_WITH_MSG, VE_INTERNAL_ASSERT_NO_MSG)(__VA_ARGS__))

#define VE_CORE_ASSERT(...) \
    VE_EXPAND_MACRO(VE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__, VE_INTERNAL_ASSERT_WITH_MSG, VE_INTERNAL_ASSERT_WITH_MSG, VE_INTERNAL_ASSERT_NO_MSG)(__VA_ARGS__))

#else

#define VE_ASSERT(...)
#define VE_CORE_ASSERT(...)

#endif