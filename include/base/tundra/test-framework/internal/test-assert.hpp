#pragma once

#include <cstdio>

#include <tundra/core/types.hpp>

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-value-to-string.hpp>
#include <tundra/test-framework/internal/test-failure-report.hpp>

namespace td::internal {

    const uint32 TEST_VALUE_STRING_BUFFER_SIZE = 128;
    static char value_1_string_buffer[TEST_VALUE_STRING_BUFFER_SIZE];
    static char value_2_string_buffer[TEST_VALUE_STRING_BUFFER_SIZE];

    const uint32 FAILURE_MESSAGE_BUFFER_SIZE = 128 + 2 * TEST_VALUE_STRING_BUFFER_SIZE;
    static char failure_message_buffer[FAILURE_MESSAGE_BUFFER_SIZE];

    template<typename T1, typename T2>
    static bool test_compare_equal(const T1& value_1, const T2& value_2) {
        return value_1 == value_2;
    }

    template<typename T1, typename T2>
    static bool test_compare_not_equal(const T1& value_1, const T2& value_2) {
        return value_1 != value_2;
    }

    template<typename T1, typename T2>
    static bool test_compare_greater(const T1& value_1, const T2& value_2) {
        return value_1 > value_2;
    }

    template<typename T1, typename T2>
    static bool test_compare_greater_or_equal(const T1& value_1, const T2& value_2) {
        return value_1 >= value_2;
    }

    template<typename T1, typename T2>
    static bool test_compare_less(const T1& value_1, const T2& value_2) {
        return value_1 < value_2;
    }

    template<typename T1, typename T2>
    static bool test_compare_less_or_equal(const T1& value_1, const T2& value_2) {
        return value_1 <= value_2;
    }

    // Returns the value of 'is_true'
    template<typename T1, typename T2>
    static bool test_assert(
        Test* test,
        const T1& value_1,
        const T2& value_2,
        const char* value_1_raw_string,
        const char* value_2_raw_string,
        bool (*comparison_func)(const T1&, const T2&),
        const char* comparison_operator,
        const char* file_name,
        uint16 line_number
    ) {
        if( comparison_func(value_1, value_2) ) return true;

        TestValueToString<T1>::to_string(value_1, value_1_string_buffer, TEST_VALUE_STRING_BUFFER_SIZE);
        TestValueToString<T2>::to_string(value_2, value_2_string_buffer, TEST_VALUE_STRING_BUFFER_SIZE);
        
        std::snprintf(
            failure_message_buffer,
            FAILURE_MESSAGE_BUFFER_SIZE,
            "%s %s %s (was %s %s %s)",
            value_1_raw_string,
            comparison_operator, 
            value_2_raw_string,
            value_1_string_buffer, 
            comparison_operator,
            value_2_string_buffer);

        Test::current_failure_report = { test, failure_message_buffer, file_name, line_number };
        Test::last_test_failed = true;

        return false;
    }
}

#define TDI_TEST_ASSERT(actual, expected, operator_string, operator_func)\
    if( !td::internal::test_assert(td_internal_input_test, actual, expected, #actual, #expected, &operator_func<decltype(actual), decltype(expected)>, operator_string, __FILE__, __LINE__) ) return;    

#define TD_TEST_ASSERT_EQUAL(actual, expected)\
    TDI_TEST_ASSERT(actual, expected, "==", td::internal::test_compare_equal)

#define TD_TEST_ASSERT_NOT_EQUAL(actual, not_expected)\
    TDI_TEST_ASSERT(actual, not_expected, "!=", td::internal::test_compare_not_equal)

#define TD_TEST_ASSERT_GREATER(actual, upper)\
    TDI_TEST_ASSERT(actual, upper, ">", td::internal::test_compare_greater)

#define TD_TEST_ASSERT_GREATER_OR_EQUAL(actual, upper)\
    TDI_TEST_ASSERT(actual, upper, ">=", td::internal::test_compare_greater_or_equal)

#define TD_TEST_ASSERT_LESS(actual, lower)\
    TDI_TEST_ASSERT(actual, lower, "<", td::internal::test_compare_less)

#define TD_TEST_ASSERT_LESS_OR_EQUAL(actual, lower)\
    TDI_TEST_ASSERT(actual, lower, "<=", td::internal::test_compare_less_or_equal)
