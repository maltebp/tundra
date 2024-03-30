#pragma once

#include <tundra/test-framework//test.hpp>
#include <tundra/test-framework//test-assert.hpp>

#include <tundra/core/types.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/fixed.hpp>


TD_TEST("vec3/constructon-and-comparison") {
    
    td::Vec2<td::Fixed32<12>> vf_0{0};
    TD_TEST_ASSERT_EQUAL(vf_0, vf_0);

    td::Vec2<td::Fixed32<12>> vf_1 { 1, 2 };
    TD_TEST_ASSERT_EQUAL(vf_1, vf_1);

    td::Vec2<td::Fixed32<12>> vf_2 { vf_1};
    TD_TEST_ASSERT_EQUAL(vf_1, vf_2);

    td::Vec2<td::int32> vi_0{0};
    TD_TEST_ASSERT_EQUAL(vi_0, vi_0);

    td::Vec2<td::int32> vi_1 { 1, 2 };
    TD_TEST_ASSERT_EQUAL(vi_1, vi_1);
}

TD_TEST("vec3/addition") {
    
    td::Vec2<td::Fixed32<12>> v1{1, 2};
    td::Vec2<td::Fixed32<12>> v2{4, 5};

    td::Vec2<td::Fixed32<12>> expected{5, 7};

    TD_TEST_ASSERT_EQUAL(v1 + v2, expected);
}

TD_TEST("vec3/subtraction") {
    
    td::Vec2<td::Fixed32<12>> v1{1, 2};
    td::Vec2<td::Fixed32<12>> v2{6, 5};

    td::Vec2<td::Fixed32<12>> expected{-5, -3};

    TD_TEST_ASSERT_EQUAL(v1 - v2, expected);
}

TD_TEST("vec3/multiply") {
    
    td::Vec2<td::Fixed32<12>> v1{1, 2};

    td::Vec2<td::Fixed32<12>> v2{2, 3};
    
    v1 *= v2;
    td::Vec2<td::Fixed32<12>> expected{2, 6};
    TD_TEST_ASSERT_EQUAL(v1, expected);
    
    v1 = v1 * v2;
    expected = {4, 18};
    TD_TEST_ASSERT_EQUAL(v1, expected);    
}

TD_TEST("vec3/divide") {

    td::Vec2<td::Fixed32<12>> expected;
    
    td::Vec2<td::Fixed32<12>> v1{2, 2};

    td::Vec2<td::Fixed32<12>> v2{2, 4};
    
    v1 /= v2;
    expected = {td::to_fixed(1), td::to_fixed(0.5)};
    TD_TEST_ASSERT_EQUAL(v1, expected);
    
    v1 = v1 / v2;
    expected = {td::to_fixed(0.5), td::to_fixed(0.125)};
    TD_TEST_ASSERT_EQUAL(v1, expected);    
}

TD_TEST("vec3/single-value-multiply") {
    
    td::Vec2<td::Fixed32<12>> v{1, 2};
    v *= 2;

    td::Vec2<td::Fixed32<12>> expected{2, 4};

    TD_TEST_ASSERT_EQUAL(v, expected);

    expected = {6, 12};

    v = v * 3;
    TD_TEST_ASSERT_EQUAL(v, expected);    
}

TD_TEST("vec3/dot-product") {
    
    {
        td::Vec2<td::Fixed32<12>> v1{2, 0};
        td::Vec2<td::Fixed32<12>> v2{0, 3};
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), 0);    
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), v2.dot(v1));    
    }

    {
        td::Vec2<td::Fixed32<12>> v1{1, 0};
        TD_TEST_ASSERT_EQUAL(v1.dot(v1), 1);    
    }

    {
        td::Vec2<td::Fixed32<12>> v1{1, 0};
        td::Vec2<td::Fixed32<12>> v2{-1, 0};
        TD_TEST_ASSERT_EQUAL(v1.dot(v2), -1);    
    }
}