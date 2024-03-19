#pragma once


#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/gte/operations.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/mat/mat3x3.hpp>

static inline constexpr td::Mat3x3 IDENTITY = td::Mat3x3<td::Fixed16<12>>::get_identity();

template<typename T>
consteval td::Mat3x3<T> from_double(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22) {
    return {
        td::to_fixed(m00), td::to_fixed(m01), td::to_fixed(m02),
        td::to_fixed(m10), td::to_fixed(m11), td::to_fixed(m12),
        td::to_fixed(m20), td::to_fixed(m21), td::to_fixed(m22),
    };
}

TD_TEST("gte/operations/multiply-matrix/identity") {
    
    td::Mat3x3 result;

    result = td::gte::multiply(IDENTITY, IDENTITY);
    TD_TEST_ASSERT_EQUAL(result, IDENTITY);

    td::Mat3x3<td::Fixed16<12>> m {
        td::to_fixed(0.125), td::to_fixed(0.250), td::to_fixed(0.5),
        td::to_fixed(0.250), td::to_fixed(0.5), td::to_fixed(0.75),
        td::to_fixed(0.5), td::to_fixed(0.75), td::to_fixed(1)
    };
    
    result = td::gte::multiply(m, IDENTITY);
    TD_TEST_ASSERT_EQUAL(result, m);

    result = td::gte::multiply(IDENTITY, m);
    TD_TEST_ASSERT_EQUAL(result, m);
}

TD_TEST("gte/operations/multiply-matrix/basic") {
    
    td::Mat3x3 result;

    td::Mat3x3<td::Fixed16<12>> m = from_double<td::Fixed16<12>>(
        0.125, 0.250, 0.5,
        0.250, 0.5, 0.75,
        0.5, 0.75, 1
    );

    td::Mat3x3<td::Fixed16<12>> expected = from_double<td::Fixed16<12>>(
        21/64.0, 17/32.0, 3/4.0,
        17/32.0, 7/8.0, 5/4.0,
        3/4.0, 5/4.0, 29/16.0
    );
    
    result = td::gte::multiply(m, m);
    TD_TEST_ASSERT_EQUAL(result, expected);
}

TD_TEST("gte/operations/normalize/no-effect") {
    
    td::Vec3<td::Fixed32<12>> v1 { 1, 0, 0 };
    td::Vec3<td::Fixed32<12>> v1_normalized = (td::Vec3<td::Fixed32<12>>)td::gte::normalize(v1);
    TD_TEST_ASSERT_EQUAL(v1_normalized, v1);

    td::Vec3<td::Fixed32<12>> v2 { 0, 1, 0 };
    td::Vec3<td::Fixed32<12>> v2_normalized = (td::Vec3<td::Fixed32<12>>)td::gte::normalize(v2);
    TD_TEST_ASSERT_EQUAL(v2_normalized, v2);

    td::Vec3<td::Fixed32<12>> v3 { 0, 0, 1 };
    td::Vec3<td::Fixed32<12>> v3_normalized = (td::Vec3<td::Fixed32<12>>)td::gte::normalize(v3);
    TD_TEST_ASSERT_EQUAL(v3_normalized, v3);    
}

TD_TEST("gte/operations/normalize/single-dimension") {
    
    td::Vec3<td::Fixed32<12>> v1 { 2, 0, 0 };
    td::Vec3<td::Fixed16<12>> v1_normalized = td::gte::normalize(v1);
    td::Vec3<td::Fixed16<12>> expected {1, 0, 0};
    TD_TEST_ASSERT_EQUAL(v1_normalized, expected);
}

TD_TEST("gte/operations/normalize/same-value ") {
    
    td::Vec3<td::Fixed32<12>> v { td::to_fixed(0.5) };
    td::Vec3<td::Fixed16<12>> v_normalized = td::gte::normalize(v);

    // Actual result is 0.5773503...
    td::Fixed16<12> min { td::to_fixed(0.577) };
    td::Fixed16<12> max { td::to_fixed(0.578) };
    
    TD_TEST_ASSERT_GREATER(v_normalized.x, min);
    TD_TEST_ASSERT_GREATER(v_normalized.y, min);
    TD_TEST_ASSERT_GREATER(v_normalized.z, min);

    TD_TEST_ASSERT_LESS(v_normalized.x, max);
    TD_TEST_ASSERT_LESS(v_normalized.y, max);
    TD_TEST_ASSERT_LESS(v_normalized.z, max);
}