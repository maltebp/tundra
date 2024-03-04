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

    // Returns the value of 'is_true'
    template<typename T1, typename T2>
    bool test_assert_equal( Test* test, const T1& value_1, const T2& value_2, const char* comparison_string, const char* file_name, uint16 line_number) {
        if( value_1 == value_2 ) return true;
        
        
        TestValueToString<T1>::to_string(value_1, value_1_string_buffer, TEST_VALUE_STRING_BUFFER_SIZE);
        TestValueToString<T2>::to_string(value_2, value_2_string_buffer, TEST_VALUE_STRING_BUFFER_SIZE);
        
        // a == c (was 10 == 7);
        std::snprintf(
            failure_message_buffer,
            FAILURE_MESSAGE_BUFFER_SIZE,
            "%s (was %s == %s)",
            comparison_string, 
            value_1_string_buffer, 
            value_2_string_buffer);

        Test::current_failure_report = { test, failure_message_buffer, file_name, line_number };
        Test::last_test_failed = true;

        return false;
    }

}


#define TD_TEST_ASSERT_EQUAL(actual, expected)\
    if( !td::internal::test_assert_equal(td_internal_input_test, actual, expected, #actual" == "#expected, __FILE__, __LINE__) ) return;    

// TODO: Implement rest:
#define TD_TEST_ASSERT_NOT_EQUAL(actual, not_expected)

#define TD_TEST_ASSERT_GREATER(actual, upper)

#define TD_TEST_ASSERT_GREATER_OR_EQUAL(actual, upper)

#define TD_TEST_ASSERT_LESS(actual, lower)

#define TD_TEST_ASSERT_LESS_OR_EQUAL(actual, lower)
