#pragma once

#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/string.hpp> // TODO: Use
#include <tundra/core/string-util.hpp>

namespace td {

    template<typename T>
    constexpr Vec3<T>::Vec3(const T& same_value) : x(same_value), y(same_value), z(same_value) { }

    template<typename T>
    constexpr Vec3<T>::Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) { }

    // TODO: Only enable this if an implicit conversion exists
    // template<typename T>
    // template<typename TOther>
    // constexpr Vec3<T>::Vec3(const Vec3<TOther>& other) : x(other.x), y(other.y), z(other.z) { }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator=(const Vec3& other) {
        x = other.x; y = other.y; z = other.z;
        return *this;    
    }

    template<typename T>
    template<typename TOther>
    [[nodiscard]] constexpr Vec3<T>::operator Vec3<TOther>() const {
        return Vec3<TOther>{ (TOther)x, (TOther)y, (TOther)z };
    }

    template<typename T>
    template<typename TOther>
    [[nodiscard]] constexpr Vec3<T>::operator Vec2<TOther>() const {
        return Vec2{ (TOther)(x), (TOther)(y) };
    }

    template<typename T>
    [[nodiscard]] constexpr bool Vec3<T>::operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    // Only returns true if it is >= is true for all elements
    template<typename T>
    [[nodiscard]] constexpr bool Vec3<T>::operator>=(const Vec3& other) const {
        return x >= other.x && y >= other.y && z >= other.z;
    }

    // Only returns true if it is <= is true for all elements
    template<typename T>
    [[nodiscard]] constexpr bool Vec3<T>::operator<=(const Vec3& other) const {
        return x <= other.x && y <= other.y && z <= other.z;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator+(const Vec3& other) const { 
        return Vec3{ x + other.x, y + other.y, z + other.z };
    }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator+=(const Vec3& other) { 
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator-(const Vec3& other) const { 
        return Vec3<T>{ x - other.x, y - other.y, z - other.z };
    }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator-=(const Vec3& other) { 
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

        template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator-() const { 
        return Vec3<T>{ -x, -y, -z };
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator*(const Vec3& other) const { 
        return Vec3{ x * other.x, y * other.y, z * other.z };
    }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator*=(const Vec3& other) { 
        x *= other.x; y *= other.y; z *= other.z;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator*(const T& value) const { 
        return Vec3{ x * value, y * value, z * value };
    }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator*=(const T& value) { 
        x *= value; y *= value; z *= value;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::operator/(const Vec3& other) const { 
        return Vec3{ x / other.x, y / other.y, z / other.z };
    }

    template<typename T>
    constexpr Vec3<T>& Vec3<T>::operator/=(const Vec3& other) { 
        x /= other.x; y /= other.y; z /= other.z;
        return *this;
    }

    template<typename T>
    [[nodiscard]] constexpr T Vec3<T>::dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    template<typename T>
    [[nodiscard]] constexpr Vec3<T> Vec3<T>::cross(const Vec3& other) const {
        // TODO: For Fixed16 I think it might be faster to subtract in the intermediate format
        return Vec3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    // TODO: Move declaration to vec3.dec.hpp once string.hpp has a string.dec.hpp
    template<typename T>
    String to_string(const Vec3<T>& v) {
        return string_util::create_from_format(
            "(%s, %s, %s)", 
            to_string(v.x).get_c_string(),
            to_string(v.y).get_c_string(),
            to_string(v.z).get_c_string()
        );
    }
    
}