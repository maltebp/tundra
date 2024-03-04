#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/internal/test-assert.hpp>

int some_function(int i) {
    return i;
}

TD_TEST("test-test-1") {
    int a = 2;
    TD_TEST_ASSERT_EQUAL(a, some_function(2));
}

TD_TEST("test-test-2") {
    int a = 2;
    TD_TEST_ASSERT_EQUAL(a, 3);
}

TD_TEST("test-test-3") {
    int a = 2;
    int* a_ptr = &a;
    TD_TEST_ASSERT_EQUAL(a_ptr, a_ptr);
}