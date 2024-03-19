#pragma once

#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/test.hpp>

#include <tundra/core/mat/mat3x3.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/fixed.hpp>

TD_TEST("mat3x3/construction-and-comparison") {
    td::Mat3x3<td::Fixed32<12>> m1 {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    td::Mat3x3<td::Fixed32<12>> m2 {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };

    TD_TEST_ASSERT_EQUAL(m1, m2);

    // Same value
    td::Mat3x3<td::Fixed32<12>> m3 { 1 };
    td::Mat3x3<td::Fixed32<12>> m4 { 
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
    };
    TD_TEST_ASSERT_EQUAL(m3, m4);

    // Copy
    td::Mat3x3<td::Fixed32<12>> m5 { m1 };
    TD_TEST_ASSERT_EQUAL(m5, m1);

    // Assignment
    m5 = m3;
    TD_TEST_ASSERT_EQUAL(m5, m3);
}

TD_TEST("mat3x3/get-set-row") {
    td::Mat3x3<td::Fixed32<12>> expected;
    td::Vec3<td::Fixed32<12>> row;
    
    td::Mat3x3<td::Fixed32<12>> m{ 0 };

    row = {1, 2, 3};
    m.set_row(0, row);
    expected = {
        1, 2, 3,
        0, 0, 0,
        0, 0, 0
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_row(0), row);

    row = {4, 5, 6};
    m.set_row(1, row);
    expected = {
        1, 2, 3,
        4, 5, 6,
        0, 0, 0
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_row(1), row);

    row = {7, 8, 9};
    m.set_row(2, row);
    expected = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_row(2), row);
}

TD_TEST("mat3x3/get-set-column") {
    td::Mat3x3<td::Fixed32<12>> expected;
    td::Vec3<td::Fixed32<12>> column;
    
    td::Mat3x3<td::Fixed32<12>> m{ 0 };

    column = {1, 2, 3};
    m.set_column(0, column);
    expected = {
        1, 0, 0,
        2, 0, 0,
        3, 0, 0
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_column(0), column);

    column = {4, 5, 6};
    m.set_column(1, column);
    expected = {
        1, 4, 0,
        2, 5, 0,
        3, 6, 0
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_column(1), column);

    column = {7, 8, 9};
    m.set_column(2, column);
    expected = {
        1, 4, 7,
        2, 5, 8,
        3, 6, 9
    };
    TD_TEST_ASSERT_EQUAL(m, expected);
    TD_TEST_ASSERT_EQUAL(m.get_column(2), column);
}

TD_TEST("mat3x3/transpose") {
    td::Mat3x3<td::Fixed32<12>> m1 {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    td::Mat3x3<td::Fixed32<12>> expected {
        { 1, 4, 7 },
        { 2, 5, 8 },
        { 3, 6, 9 }
    };

    m1.transpose();
    TD_TEST_ASSERT_EQUAL(m1, expected);


    td::Mat3x3<td::Fixed32<12>> transposed_identity = td::Mat3x3<td::Fixed32<12>>::get_identity();
    transposed_identity.transpose();
    TD_TEST_ASSERT_EQUAL(transposed_identity, td::Mat3x3<td::Fixed32<12>>::get_identity());
}

