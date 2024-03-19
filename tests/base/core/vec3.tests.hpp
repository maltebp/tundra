#pragma once

#include "tundra/core/vec/vec3.fwd.hpp"
#include <tundra/test-framework//test.hpp>
#include <tundra/test-framework//test-assert.hpp>

#include <tundra/core/types.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/fixed.hpp>


TD_TEST("vec3/constructon-and-comparison") {
    
    td::Vec3<td::Fixed32<12>> vf_0{0};
    TD_TEST_ASSERT_EQUAL(vf_0, vf_0);

    td::Vec3<td::Fixed32<12>> vf_1 { 1, 2, 3 };
    TD_TEST_ASSERT_EQUAL(vf_1, vf_1);

    td::Vec3<td::Fixed32<12>> vf_2 { vf_1};
    TD_TEST_ASSERT_EQUAL(vf_1, vf_2);

    td::Vec3<td::int32> vi_0{0};
    TD_TEST_ASSERT_EQUAL(vi_0, vi_0);

    td::Vec3<td::int32> vi_1 { 1, 2, 3 };
    TD_TEST_ASSERT_EQUAL(vi_1, vi_1);
}

TD_TEST("vec3/from-vec2") {

    td::Vec2<td::Fixed32<12>> vec2{ 1, 2 };

    td::Vec3<td::Fixed32<12>> vec3 = (td::Vec3<td::Fixed32<12>>)vec2;
    
    td::Vec3<td::Fixed32<12>> expected { 1, 2, 0 };
    TD_TEST_ASSERT_EQUAL(vec3, expected);
}

TD_TEST("vec3/addition") {
    
    td::Vec3<td::Fixed32<12>> v1{1, 2, 3};
    td::Vec3<td::Fixed32<12>> v2{4, 5, 6};

    td::Vec3<td::Fixed32<12>> expected{5, 7, 9};

    TD_TEST_ASSERT_EQUAL(v1 + v2, expected);
}

TD_TEST("vec3/subtraction") {
    
    td::Vec3<td::Fixed32<12>> v1{1, 2, 3};
    td::Vec3<td::Fixed32<12>> v2{6, 5, 4};

    td::Vec3<td::Fixed32<12>> expected{-5, -3, -1};

    TD_TEST_ASSERT_EQUAL(v1 - v2, expected);

    expected = { -1, -2 , -3 };
    TD_TEST_ASSERT_EQUAL(-v1, expected);
}

TD_TEST("vec3/multiply") {
    
    td::Vec3<td::Fixed32<12>> v1{1, 2, 3};

    td::Vec3<td::Fixed32<12>> v2{2, 3, 4};
    
    v1 *= v2;
    td::Vec3<td::Fixed32<12>> expected{2, 6, 12};
    TD_TEST_ASSERT_EQUAL(v1, expected);
    
    v1 = v1 * v2;
    expected = {4, 18, 48};
    TD_TEST_ASSERT_EQUAL(v1, expected);    
}

TD_TEST("vec3/divide") {

    td::Vec3<td::Fixed32<12>> expected;
    
    td::Vec3<td::Fixed32<12>> v1{2, 2, 2};

    td::Vec3<td::Fixed32<12>> v2{2, 4, 8};
    
    v1 /= v2;
    expected = {td::to_fixed(1), td::to_fixed(0.5), td::to_fixed(0.25)};
    TD_TEST_ASSERT_EQUAL(v1, expected);
    
    v1 = v1 / v2;
    expected = {td::to_fixed(0.5), td::to_fixed(0.125), td::to_fixed(0.03125)};
    TD_TEST_ASSERT_EQUAL(v1, expected);    
}

TD_TEST("vec3/single-value-multiply") {
    
    td::Vec3<td::Fixed32<12>> v{1, 2, 3};
    v *= 2;

    td::Vec3<td::Fixed32<12>> expected{2, 4, 6};

    TD_TEST_ASSERT_EQUAL(v, expected);

    expected = {6, 12, 18};

    v = v * 3;
    TD_TEST_ASSERT_EQUAL(v, expected);    
}

TD_TEST("vec3/dot-product") {
    
    {
        td::Vec3<td::Fixed32<12>> v1{2, 0, 0};
        td::Vec3<td::Fixed32<12>> v2{0, 3, 0};
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), 0);    
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), v2.dot(v1));    
    }

    {
        td::Vec3<td::Fixed32<12>> v1{1, 0, 0};
        TD_TEST_ASSERT_EQUAL(v1.dot(v1), 1);    
    }

    {
        td::Vec3<td::Fixed32<12>> v1{1, 0, 0};
        td::Vec3<td::Fixed32<12>> v2{-1, 0, 0};
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), -1);    
    }
}

TD_TEST("vec3/cross-product") {
    
        td::Vec3<td::Fixed32<12>> x_axis{1, 0, 0};
        td::Vec3<td::Fixed32<12>> y_axis{0, 1, 0};
        
        td::Vec3<td::Fixed32<12>> z_axis{0, 0, 1};
        TD_TEST_ASSERT_EQUAL(x_axis.cross(y_axis), z_axis);    
        TD_TEST_ASSERT_EQUAL(y_axis.cross(x_axis), -z_axis);    
}