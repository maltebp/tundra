#pragma once

namespace td {

    template<class T> struct remove_reference { typedef T type; };
    template<class T> struct remove_reference<T&> { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };

    template<typename  T>
    constexpr remove_reference<T>::type&& move(T& t) noexcept {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }

    template<typename T> consteval bool is_unsigned();
    template<typename T> consteval bool is_signed() { return !is_unsigned<T>(); }

    template<> consteval bool is_unsigned<char>() { return false; };
    template<> consteval bool is_unsigned<short>() { return false; };
    template<> consteval bool is_unsigned<int>() { return false; };
    template<> consteval bool is_unsigned<long>() { return false; };
    template<> consteval bool is_unsigned<long long>() { return false; };

    template<> consteval bool is_unsigned<unsigned char>() { return true; };
    template<> consteval bool is_unsigned<unsigned short>() { return true; };
    template<> consteval bool is_unsigned<unsigned int>() { return true; };  
    template<> consteval bool is_unsigned<unsigned long>() { return true; };
    template<> consteval bool is_unsigned<unsigned long long>() { return true; };
}