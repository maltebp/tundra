#pragma once

#include <cstdio>

#ifdef TD_PLATFORM_PLAYSTATION
#include <exception>
#endif

#ifdef TD_NO_ASSERTS

#define TD_ASSERT(expr, format, ...)

#else

namespace {

    // Should not be called by code manually
    template<typename ... TArgs>
    static void _handle_assert (const char *format, TArgs... args) {
        std:: printf(format, args...);

#if defined TD_PLATFORM_PLAYSTATION
        for (;;) {
            __asm__ volatile("");
        }
#elif defined TD_PLATFORM_DEVELOPMENT
        std::terminate();
#else
#error "No platform is set"
#endif
    }

}

#define TD_ASSERT(expression, format, ...) \
	((expression) ? ((void) 0) : _handle_assert("Assert failed in " __FILE__ ":%d: "  #expression " (" format ")\n", __LINE__ __VA_OPT__(,) __VA_ARGS__)) \

#endif