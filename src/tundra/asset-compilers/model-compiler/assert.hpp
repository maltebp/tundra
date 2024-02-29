#pragma once

#include <cstdio>
#include <exception>

namespace td :: ac {

    class InputException : public std::exception {
        using std::exception::exception;
    };

    // Not an assert that will be stripped from compilation, but a shorthand for
    // throwing an exception
    template<typename ... TArgs>
    static void assert(bool check, const char* format, const TArgs& ... args) {
        // This could probably be done better

        if( check ) return;

        static char format_buffer[500];
        std::snprintf(format_buffer, 500, format, args...);
        throw InputException(format_buffer);
    }
}