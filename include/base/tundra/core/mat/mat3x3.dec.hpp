#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vec/vec3.fwd.hpp>
#include <tundra/core/mat/mat3x3.fwd.hpp>

#include <tundra/core/fixed.fwd.hpp>

namespace td {

    template<typename T = Fixed16<12>>
    class Mat3x3 {
    public:
    
        constexpr Mat3x3() = default;

        constexpr Mat3x3(const Mat3x3& other) = default;

        constexpr Mat3x3(Mat3x3&& other) = default;

        constexpr ~Mat3x3() = default;

        constexpr explicit Mat3x3(T same_value);

        constexpr Mat3x3(
            T m00, T m01, T m02,
            T m10, T m11, T m12,
            T m20, T m21, T m22
        );

        constexpr Mat3x3(const Vec3<T>& row0, const Vec3<T>& row1, const Vec3<T>& row2);

        // TODO: Only enable this if there is an implicit conversion from TOther to T (and both are not signed)
        // template<typename TOther>
        // constexpr Mat3x3(const Mat3x3<TOther>& other);

        template<typename TOther>
        [[nodiscard]] constexpr explicit operator Mat3x3<TOther>() const;

        constexpr Mat3x3<T>& operator=(const Mat3x3& other) = default;

        [[nodiscard]] constexpr bool operator==(const Mat3x3& other) const;

        // TODO: Implement regular multiplication
        // Why is there no multiply atm? A "fast" multiply on PS side is only available for 3x3
        // matrix in fixed 4.12 format, and using that will overwrite the current rotation or
        // light matrix. So this operation is instead explictly implemented on PlayStation
        // for that specific instantiation. I expect multiplication of other types not to be
        // relevant for the time being, so no general-purpose multiplication using regular registers
        // is implemented atm.

        constexpr void set_row(uint32 row_index, const Vec3<T>& row_values);

        // TODO: We could have a reference version of this
        constexpr Vec3<T> get_row(uint32 row_index) const;

        constexpr void set_column(uint32 column_index, const Vec3<T>& column_values);

        constexpr Vec3<T> get_column(uint32 column_index) const;       

        constexpr void transpose();

        static inline constexpr Mat3x3<T> get_identity() {
            return {
                1, 0, 0,
                0, 1, 0,
                0, 0, 1  
            };
        }

        // TODO: Left out a bunch of operations (add, subtract, etc), so implement
        // those when I have the time

        T m00, m01, m02; // Row 1
        T m10, m11, m12; // Row 2
        T m20, m21, m22; // Row 3

    };

}