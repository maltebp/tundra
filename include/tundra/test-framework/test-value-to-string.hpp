#pragma once

#include <cstdio>

#include <tundra/core/types.hpp>

namespace td {

    template<typename T>
    class TestValueToString {
    public:
        static void to_string(const T& t, char* buffer, int16 buffer_size) {
            std::snprintf(buffer, buffer_size, "<no-string-conversion>");
        }
    };

    template<>
    class TestValueToString<int> {
    public:
        static void to_string(const int& t, char* buffer, int16 buffer_size) {
            std::snprintf(buffer, buffer_size, "%d", t);
        }
    };

    template<>
    class TestValueToString<unsigned int> {
    public:
        static void to_string(const unsigned int& t, char* buffer, int16 buffer_size) {
            std::snprintf(buffer, buffer_size, "%d", t);
        }
    };

    template<>
    class TestValueToString<bool> {
    public:
        static void to_string(const bool& t, char* buffer, int16 buffer_size) {
            std::snprintf(buffer, buffer_size, (t ? "true" : "false"));
        }
    };

    // I don't entirely grasp how this works - I tried doing it with a function 
    // instead of a class, but then pointer templates were not resolved properly
    template<typename T>
    class TestValueToString<T*> {
    public:
        static void to_string(const T* t, char* buffer, int16 buffer_size) {
            std::snprintf(buffer, buffer_size, "%p", t);
        }
    };
}