#pragma once

#include <tundra/core/vec/vec2.dec.hpp>
#include <tundra/core/vec/vec3.fwd.hpp>

namespace td {

    template<typename T>
    class Vec3 {
    public:

        constexpr Vec3() = default;

        constexpr Vec3(const Vec3& other) = default;

        constexpr Vec3(Vec3&&) = default;

        constexpr ~Vec3() = default;

        constexpr explicit Vec3(const T& same_value);

        constexpr Vec3(const T& x, const T& y, const T& z);

        // TODO: Only enable this if an implicit conversion exists
        // template<typename TOther>
        // constexpr Vec3(const Vec3<TOther>& other);

        constexpr Vec3& operator=(const Vec3& other);

        template<typename TOther>
        [[nodiscard]] constexpr explicit operator Vec3<TOther>() const;

        template<typename TOther>
        [[nodiscard]] constexpr explicit operator Vec2<TOther>() const;

        [[nodiscard]] constexpr bool operator==(const Vec3& other) const;

        // Only returns true if it is >= is true for all elements
        [[nodiscard]] constexpr bool operator>=(const Vec3& other) const;

        // Only returns true if it is <= is true for all elements
        [[nodiscard]] constexpr bool operator<=(const Vec3& other) const;

        [[nodiscard]] constexpr Vec3 operator+(const Vec3& other) const;
        constexpr Vec3& operator+=(const Vec3& other);

        [[nodiscard]] constexpr Vec3 operator-() const;

        [[nodiscard]] constexpr Vec3 operator-(const Vec3& other) const;
        constexpr Vec3& operator-=(const Vec3& other);

        [[nodiscard]] constexpr Vec3 operator*(const Vec3& other) const ;
        constexpr Vec3& operator*=(const Vec3& other);

        [[nodiscard]] constexpr Vec3 operator*(const T& value) const ;
        constexpr Vec3& operator*=(const T& value);

        [[nodiscard]] constexpr Vec3 operator/(const Vec3& other) const;
        constexpr Vec3& operator/=(const Vec3& other);

        [[nodiscard]] constexpr T dot(const Vec3& other) const;

        [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const;

        T x;
        T y;
        T z;

    };

}