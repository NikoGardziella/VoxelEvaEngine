#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

#ifdef VE_DEBUG

#define VE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
        { \
            if (!(check)) \
            { \
                ENGINE_ERROR(msg, __VA_ARGS__); \
                VE_DEBUGBREAK(); \
            } \
        }

#define VE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
        VE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)

#define VE_INTERNAL_ASSERT_NO_MSG(type, check) \
        VE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", \
            VE_STRINGIFY_MACRO(check), __FILE__, __LINE__)

#define VE_ASSERT(check, ...) \
        VE_EXPAND_MACRO(VE_INTERNAL_ASSERT_NO_MSG(_, check))

#define VE_CORE_ASSERT(check, ...) \
        VE_EXPAND_MACRO(VE_INTERNAL_ASSERT_NO_MSG(_, check))

#else

#define VE_ASSERT(check, ...)
#define VE_CORE_ASSERT(check, ...)

#endif