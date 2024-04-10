#pragma once

#include <tundra/core/string-util.hpp>
#include <tundra/core/string.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <type_traits>

namespace td {

    template<typename T>
    constexpr Vec2<T>::Vec2(const T& same_value) : x(same_value), y(same_value) { }

    template<typename T>
    constexpr Vec2<T>::Vec2(const T& x, const T& y) : x(x), y(y) { }

    // TODO: Only enable this if an implicit conversion exists
    template<typename T>
    template<typename TOther>
    constexpr Vec2<T>::Vec2(const Vec2<TOther>& other) 
        requires( 
                std::is_convertible<TOther, T>::value 
            && (std::is_unsigned<T>::value == std::is_unsigned<TOther>::value 
                    || sizeof(T) > sizeof(TOther))) 
        : x(other.x), y(other.y) { }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator=(const Vec2& other) {
        x = other.x; y = other.y;
        return *this;    
    }

    template<typename T>
    template<typename TOther>
    [[nodiscard]] constexpr Vec2<T>::operator Vec2<TOther>() const {
        return Vec2<TOther>{ static_cast<TOther>(x), static_cast<TOther>(y) };
    }

    template<typename T>
    template<typename TOther>
    [[nodiscard]] constexpr Vec2<T>::operator Vec3<TOther>() const {
        return Vec3{ static_cast<TOther>(x), static_cast<TOther>(y), TOther{} };
    }

    template<typename T>
    [[nodiscard]] constexpr bool Vec2<T>::operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec2<T> Vec2<T>::operator+(const Vec2<T>& other) const { 
        return Vec2{ (T)(x + other.x), (T)(y + other.y) };
    }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator+=(const Vec2& other) { 
        x += other.x; y += other.y;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec2<T> Vec2<T>::operator-(const Vec2& other) const { 
        return Vec2{ x - other.x, y - other.y };
    }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator-=(const Vec2& other) { 
        x -= other.x; y -= other.y;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec2<T> Vec2<T>::operator*(const Vec2& other) const { 
        return Vec2{ x * other.x, y * other.y };
    }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator*=(const Vec2& other) { 
        x *= other.x; y *= other.y;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec2<T> Vec2<T>::operator*(const T& value) const { 
        return Vec2{ x * value, y * value };
    }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator*=(const T& value) { 
        x *= value; y *= value;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec2<T> Vec2<T>::operator/(const Vec2& other) const { 
        return Vec2{ x / other.x, y / other.y };
    }

    template<typename T>
    constexpr Vec2<T>& Vec2<T>::operator/=(const Vec2& other) { 
        x /= other.x; y /= other.y;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr T Vec2<T>::dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }

    // TODO: Move declaration to vec3.dec.hpp once string.hpp has a string.dec.hpp
    template<typename T>
    String to_string(const Vec2<T>& v) {
        return string_util::create_from_format(
            "(%s, %s)", 
            to_string(v.x).get_c_string(),
            to_string(v.y).get_c_string()
        );
    }
}