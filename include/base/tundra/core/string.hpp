#pragma once

#include <tundra/core/types.hpp>

namespace td {

    // For now, this is mostly for debugging purposes and not intended to be
    // particularly performant (e.g. no small-string optimization)
    class String {
    public:

        String() { };

        // Copies the C-string up until the null-terminator
        String(const char* c_string);

        // The c_string must be dynamically allocated, and the String takes
        // ownership of allocated memory (it will delete it on destruction)
        String(char* c_string, uint32 length);

        String(const String& other);

        String(String&& other);

        ~String();

        uint32 get_size() const { return size; }

        bool is_empty() const { return size > 0; }
    
        bool is_empty_or_whitespace() const;

        const char* get_c_string() const { return size > 0 ? characters : ""; }

        bool operator==(const String& other) const;

        bool operator!=(const String& other) const;

        bool operator==(const char* c_string) const;

        bool operator!=(const char* c_string) const;

        String& operator=(const String& other);

        String& operator=(const char* other);

        String& operator+=(const String& other);

        String& operator+=(const char* c_string);

        String operator+(const String& other) const;

        String operator+(const char* c_string) const;

    private:

        void set(const char* c_string, uint32 size);

        // Creates a new string that is the size of this string + the size of the
        // parameter string. Thw new string is dynamically allocated.
        char* concat_to_new(const char* c_string_1, uint32 size) const;

        char* characters = nullptr;

        uint32 size = 0;

    };

    // template<typename T>
    // String print(const T& value);
    // //  {
    // //     std::cout << "Value: " << value << std::endl;
    // // }

    // template<>
    // String print<int>(const int& value);

    namespace internal {
        String ptr_to_string(const void* ptr);
    }

    template<typename T>
    String to_string(const T&);

    template<>
    String to_string<bool>(const bool& value);

    template<>
    String to_string<char>(const char& value);

    template<>
    String to_string<short>(const short& value);

    template<>
    String to_string<int>(const int& value);

    template<>
    String to_string<long>(const long& value);

    template<>
    String to_string<long long>(const long long& value);

    template<>
    String to_string<unsigned short>(const unsigned short& value);

    template<>
    String to_string<unsigned int>(const unsigned int& value);

    template<>
    String to_string<unsigned long>(const unsigned long& value);

    template<>
    String to_string<unsigned long long>(const unsigned long long& value);

    template<>
    String to_string<String>(const String& string);

    template<typename T>
    String to_string(T* ptr) {
        // This template variant is required, otherwise non-const pointers will 
        // use the const T& variant (even if const T* exists)
        return internal::ptr_to_string(ptr);
    }

    template<typename T>
    String to_string(const T* ptr) {
        // This template variant is required, otherwise const pointers will use 
        //the const T& variant
        return internal::ptr_to_string(ptr);
    }

    template<>
    String to_string<char>(const char* string);

}