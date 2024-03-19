#pragma once

#include <tundra/core/vec/vec3.fwd.hpp>

namespace td {

    template<typename T>
    class Vec2 {
    public:

        constexpr Vec2() = default;

        constexpr Vec2(const Vec2&) = default;

        constexpr Vec2(Vec2&&) = default;

        constexpr ~Vec2() = default;

        constexpr explicit Vec2(const T& same_value);

        constexpr Vec2(const T& x, const T& y);

        // TODO: Only enable this if an implicit conversion exists
        template<typename TOther>
        constexpr Vec2(const Vec2<TOther>& other);

        constexpr Vec2& operator=(const Vec2& other);

        template<typename TOther>
        [[nodiscard]] constexpr explicit operator Vec2<TOther>() const;

        template<typename TOther>
        [[nodiscard]] constexpr explicit operator Vec3<TOther>() const;

        [[nodiscard]] constexpr bool operator==(const Vec2& other) const;

        [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const;
        constexpr Vec2& operator+=(const Vec2& other);

        [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const;
        constexpr Vec2& operator-=(const Vec2& other);

        [[nodiscard]] constexpr Vec2 operator*(const Vec2& other) const ;
        constexpr Vec2& operator*=(const Vec2& other);

        [[nodiscard]] constexpr Vec2 operator*(const T& value) const ;
        constexpr Vec2& operator*=(const T& value);

        [[nodiscard]] constexpr Vec2 operator/(const Vec2& other) const;
        constexpr Vec2& operator/=(const Vec2& other);

        [[nodiscard]] constexpr T dot(const Vec2& other) const;

        [[nodiscard]] constexpr Vec2 cross(const Vec2& other) const;

        T x;
        T y;

    };

}