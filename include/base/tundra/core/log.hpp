#pragma once

#ifdef TD_NO_DEBUG_LOG
    
    #define TD_DEBUG_LOG(message, ...)

#else

    #include <cstdio>

    #include <tundra/core/string.hpp>

    namespace td::internal {

        // We would be better served with some proper logging/stream class instead
        // of these template shenanigans (this is just because I don't have time)

        // Reduce bloat in un-optimized builds: 
        // Have explicit functions for primitives (i.e. only conver_to_string_if_object
        // is a template function), and have them in a source file (to prevent
        // multiple definitions).

        template<typename T>
        const T& to_print_value(const T& value) { return value; }

        static const char* to_print_value(const td::String& string) {
            std::printf("Using string print value"); 
            return string.get_c_string(); 
        }

        template<typename T>
        static td::String convert_to_string_if_object(const T& o) { 
            return td::to_string(o);
        }

        static char convert_to_string_if_object(const char& value) { return value; }
        static short convert_to_string_if_object(const short& value) { return value; }
        static int convert_to_string_if_object(const int& value) { return value; }
        static long convert_to_string_if_object(const long& value) { return value; }
        static long long convert_to_string_if_object(const long long& value) { return value; }
        static unsigned short convert_to_string_if_object(const unsigned short& value) { return value; }
        static unsigned int convert_to_string_if_object(const unsigned int& value) { return value; }
        static unsigned long convert_to_string_if_object(const unsigned  long& value) { return value; }
        static unsigned long long convert_to_string_if_object(const unsigned long long& value) { return value; }

        template<typename ... TArgs>
        void print_strings(const char* message, TArgs... args) {
            // Converts all primitives to the same value, and td::String to const char*
            std::printf(message, to_print_value(args)...);
            std::printf("\n");
        }

        template<typename ... TArgs>
        void log(const char* message, TArgs... args) {
            // Converts all primitives to the same value, and non-primitives to a td::String
            print_strings(message, convert_to_string_if_object(args)...);
        }
    }

    // printf format, with the extension that '%s' format works for classes for
    // which there is a specialization of td::to_string<T> defined
    #define TD_DEBUG_LOG(...)\
        td::internal::log(__VA_ARGS__);

#endif

