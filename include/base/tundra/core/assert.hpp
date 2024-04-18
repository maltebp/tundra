#pragma once

#include <cstdio>

#ifdef TD_PLATFORM_DEVELOPER
#include <exception>
#endif

#ifdef TD_NO_ASSERTS

#define TD_ASSERT(expr, format, ...)

#else

namespace td::internal {

    // Should not be called by code manually
    template<typename ... TArgs>
    [[noreturn]] static void _handle_assert (const char *format, TArgs... args) {

#if defined TD_PLATFORM_PLAYSTATION
        std::printf(format, args...);
        for (;;) {  
            __asm__ volatile("");
        }
#elif defined TD_PLATFORM_DEVELOPER
        std::fprintf(stderr, format, args...);
        throw std::exception("Assert was false");
#else
#error "No platform is set"
#endif
    }

}

#define TD_ASSERT(expression, format, ...) \
	((expression) ? ((void) 0) : td::internal::_handle_assert("Assert failed in " __FILE__ ":%d: "  #expression " (" format ")\n", __LINE__ __VA_OPT__(,) __VA_ARGS__)) \

#endif