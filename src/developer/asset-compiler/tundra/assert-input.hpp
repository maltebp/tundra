#pragma once

#include <cstdio>
#include <exception>

namespace td :: ac {

    class InputException : public std::exception {
        using std::exception::exception;
    };

    
    template<typename T>
    static const T& correct_format_arg(const T& t) {
        return t;
    }

    static const char* correct_format_arg(const std::string& s) {
        return s.c_str();
    }

    // Not an assert that will be stripped from compilation, but a shorthand for
    // throwing an exception
    template<typename ... TArgs>
    static void input_assert(bool check, const char* format, const TArgs& ... args) {
        // This could probably be done better

        if( check ) return;

        static char format_buffer[500];
        std::snprintf(format_buffer, 500, format, correct_format_arg(args)...);
        throw InputException(format_buffer);
    }

    // Not an assert that will be stripped from compilation, but a shorthand
    // for checking condition and printing warning to stderr if not met
    template<typename ... TArgs>
    static void input_assert_warning(bool check, const char* format, const TArgs& ... args) {
        if( check ) return;   

        static char format_buffer[500];
        std::snprintf(format_buffer, 500, format, correct_format_arg(args)...);
        std::cerr << "Warning: " << format_buffer << std::endl;
    }
}