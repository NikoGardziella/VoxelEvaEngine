#pragma once

#ifdef VE_PLATFORM_WINDOWS
#define VE_DEBUGBREAK() __debugbreak()
#else
#define VE_DEBUGBREAK()
#endif

#define VE_EXPAND_MACRO(x) x
#define VE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)