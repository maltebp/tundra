#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/internal/test-assert.hpp>

static int some_function(int i) {
    return i;
}

TD_TEST("test-test-1") {
    int a = 2;
    TD_TEST_ASSERT_EQUAL(a, some_function(2));
}

TD_TEST("test-test-2") {
    int a = 2;
    TD_TEST_ASSERT_EQUAL(a, 2);
}

TD_TEST("test-test-3") {
    int a = 2;
    int* a_ptr = &a;
    TD_TEST_ASSERT_EQUAL(a_ptr, a_ptr);
}

TD_TEST("test-equal") {
    int a = 2;
    TD_TEST_ASSERT_EQUAL(a, 2);
    TD_TEST_ASSERT_EQUAL(a, 1);
}

TD_TEST("test-not-equal") {
    int a = 2;
    TD_TEST_ASSERT_NOT_EQUAL(a, 1);
    TD_TEST_ASSERT_NOT_EQUAL(a, 2);
}

TD_TEST("test-greater-than") {
    int a = 2;
    TD_TEST_ASSERT_GREATER(a, 1);
    TD_TEST_ASSERT_GREATER(a, 3);
}

TD_TEST("test-greater-or-equal") {
    int a = 2;
    TD_TEST_ASSERT_GREATER(a, 1);
    TD_TEST_ASSERT_GREATER(a, 2);
    TD_TEST_ASSERT_GREATER(a, 3);
}

TD_TEST("test-less-than") {
    int a = 2;
    TD_TEST_ASSERT_LESS(a, 3);
    TD_TEST_ASSERT_LESS(a, 1);
}

TD_TEST("test-less-or-equal") {
    int a = 2;
    TD_TEST_ASSERT_LESS(a, 3);
    TD_TEST_ASSERT_LESS(a, 2);
    TD_TEST_ASSERT_LESS(a, 1);
}