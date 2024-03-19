#pragma once

#include "tundra/core/fixed.hpp"
#include <tundra/core/mat/mat3x3.dec.hpp>

#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/string.hpp>
#include <tundra/core/string-util.hpp>

namespace td {

    template<typename T>
    constexpr Mat3x3<T>::Mat3x3(T same_value) 
        :   m00(same_value), m01(same_value), m02(same_value), 
            m10(same_value), m11(same_value), m12(same_value), 
            m20(same_value), m21(same_value), m22(same_value)
    { }

    template<typename T>
    constexpr Mat3x3<T>::Mat3x3(
        T m00, T m01, T m02,
        T m10, T m11, T m12,
        T m20, T m21, T m22
    ) 
        :   m00(m00), m01(m01), m02(m02), 
            m10(m10), m11(m11), m12(m12), 
            m20(m20), m21(m21), m22(m22)
    { }

    template<typename T>
    constexpr Mat3x3<T>::Mat3x3(const Vec3<T>& row0, const Vec3<T>& row1, const Vec3<T>& row2) 
        :   m00(row0.x), m01(row0.y), m02(row0.z), 
            m10(row1.x), m11(row1.y), m12(row1.z), 
            m20(row2.x), m21(row2.y), m22(row2.z)
    { }

    // See header for why this is commented out atm
    // template<typename T>
    // template<typename TOther>
    // constexpr Mat3x3<T>::Mat3x3(const Mat3x3<TOther>& other)
    //     :   m00((T)other.m00), m01((T)other.m01), m02((T)other.m02), 
    //         m10((T)other.m10), m11((T)other.m11), m12((T)other.m12), 
    //         m20((T)other.m20), m21((T)other.m21), m22((T)other.m22)
    // { }

    template<typename T>
    template<typename TOther>
    [[nodiscard]] constexpr Mat3x3<T>::operator Mat3x3<TOther>() const {
        return Mat3x3<T> {
            (TOther)m00, (TOther)m01, (TOther)m02, 
            (TOther)m10, (TOther)m11, (TOther)m12, 
            (TOther)m20, (TOther)m21, (TOther)m22
        };
    }

    template<typename T>
    [[nodiscard]] constexpr bool Mat3x3<T>::operator==(const Mat3x3& other) const {
        return  m00 == other.m00 && m01 == other.m01 && m02 == other.m02 && 
                m10 == other.m10 && m11 == other.m11 && m12 == other.m12 && 
                m20 == other.m20 && m21 == other.m21 && m22 == other.m22;
    }

    template<typename T>
    constexpr void Mat3x3<T>::set_row(uint32 row_index, const Vec3<T>& row_values) {
        TD_ASSERT(row_index < 3, "Matrix row-index is out of bounds (was %d)", row_index);
        if( row_index == 0 ) {
            m00 = row_values.x; m01 = row_values.y; m02 = row_values.z; 
        }
        else if( row_index == 1 ) {
            m10 = row_values.x; m11 = row_values.y; m12 = row_values.z;
        }
        else {
            m20 = row_values.x; m21 = row_values.y; m22 = row_values.z;
        }
    }

    template<typename T>
    constexpr Vec3<T> Mat3x3<T>::get_row(uint32 row_index) const {
        TD_ASSERT(row_index < 3, "Matrix row-index is out of bounds (was %d)", row_index);
        if( row_index == 0 ) {
            return Vec3<T>{ m00, m01, m02 };
        }
        else if( row_index == 1 ) {
            return Vec3<T>{ m10, m11, m12 };
        }
        else {
            return Vec3<T>{ m20, m21, m22 };
        }
    }

    template<typename T>
    constexpr void Mat3x3<T>::set_column(uint32 column_index, const Vec3<T>& column_values) {
        TD_ASSERT(column_index < 3, "Matrix column-index is out of bounds (was %d)", column_index);
        if( column_index == 0 ) {
            m00 = column_values.x; 
            m10 = column_values.y; 
            m20 = column_values.z; 
        }
        else if( column_index == 1 ) {
            m01 = column_values.x;
            m11 = column_values.y;
            m21 = column_values.z;
        }
        else {
            m02 = column_values.x;
            m12 = column_values.y;
            m22 = column_values.z;
        }
    }

    template<typename T>
    constexpr Vec3<T> Mat3x3<T>::get_column(uint32 column_index) const {
        TD_ASSERT(column_index < 3, "Matrix column-index is out of bounds (was %d)", column_index);
        if( column_index == 0 ) {
            return Vec3<T>{
                m00,
                m10,
                m20
            };
        }
        else if( column_index == 1 ) {
            return Vec3<T>{
                m01,
                m11,
                m21
            };
        }
        else {
            return Vec3<T>{
                m02,
                m12,
                m22
            };
        }
    }

    template<typename T>
    constexpr void Mat3x3<T>::transpose() {
        constexpr auto flip = [](T& value1, T& value2 ) {
            T temporary = value1;
            value1 = value2;
            value2 = temporary;
        };

        flip(m01, m10);
        flip(m20, m02);
        flip(m21, m12);
    }

    template<typename T>
    String to_string(const Mat3x3<T>& m) {
        return string_util::create_from_format(
            "(%s, %s, %s)", 
            td::to_string(m.get_row(0)).get_c_string(), 
            td::to_string(m.get_row(1)).get_c_string(),
             td::to_string(m.get_row(2)).get_c_string()
        );
    }

}