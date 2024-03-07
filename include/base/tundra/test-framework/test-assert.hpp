#pragma once

#include <cstdio>

#include <tundra/core/types.hpp>
#include <tundra/core/string.hpp>

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/internal/test-failure-report.hpp>

namespace td::internal {

    template<typename T1, typename T2>
    struct TestArgs {
        Test* test;
        const T1& value_1;
        const T2& value_2;
        const char* value_1_raw_string;
        const char* value_2_raw_string;
        const char* file_name;
        uint16 line_number;
    };

    const uint32 FAILURE_MESSAGE_BUFFER_SIZE = 516;
    static char failure_message_buffer[FAILURE_MESSAGE_BUFFER_SIZE];

    template<typename T1, typename T2>
    static void report_failure(const TestArgs<T1, T2>& test_args, const char* comparison_operator) {
        td::String value_1_string = td::to_string(test_args.value_1);
        td::String value_2_string = td::to_string(test_args.value_2);
        
        std::snprintf(
            failure_message_buffer,
            FAILURE_MESSAGE_BUFFER_SIZE,
            "%s %s %s (was %s %s %s)",
            test_args.value_1_raw_string,
            comparison_operator, 
            test_args.value_2_raw_string,
            value_1_string.get_c_string(), 
            comparison_operator,
            value_2_string.get_c_string());

        Test::current_failure_report = { test_args.test, failure_message_buffer, test_args.file_name, test_args.line_number };
        Test::last_test_failed = true;
    }

    template<typename T1, typename T2>
    static bool test_assert_equal(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 == test_args.value_2 ) return true;
        report_failure(test_args, "==");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert_not_equal(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 != test_args.value_2 ) return true;
        report_failure(test_args, "!=");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert_greater(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 > test_args.value_2 ) return true;
        report_failure(test_args, ">");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert_greater_or_equal(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 >= test_args.value_2 ) return true;
        report_failure(test_args, ">=");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert_less(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 < test_args.value_2 ) return true;
        report_failure(test_args, "<");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert_less_or_equal(const TestArgs<T1, T2>& test_args) {
        if( test_args.value_1 <= test_args.value_2 ) return true;
        report_failure(test_args, "<=");
        return false;
    }

    template<typename T1, typename T2>
    static bool test_assert(const TestArgs<T1, T2>& test_args) {
        if( comparison_func(test_args.value_1, test_args.value_2) ) return true;
        report_failure(test_args, "==");
        return false;
    }
}

#define TDI_TEST_ARGS(actual, expected)\
    td::internal::TestArgs{td_internal_input_test, actual, expected, #actual, #expected, __FILE__, __LINE__}

#define TD_TEST_ASSERT_EQUAL(actual, expected)\
    if( !td::internal::test_assert_equal(TDI_TEST_ARGS(actual, expected)) ) return

#define TD_TEST_ASSERT_NOT_EQUAL(actual, not_expected)\
    if( !td::internal::test_assert_not_equal(TDI_TEST_ARGS(actual, not_expected)) ) return

#define TD_TEST_ASSERT_GREATER(actual, upper)\
    if( !td::internal::test_assert_greater(TDI_TEST_ARGS(actual, upper)) ) return

#define TD_TEST_ASSERT_GREATER_OR_EQUAL(actual, upper)\
    if( !td::internal::test_assert_greater_or_equal(TDI_TEST_ARGS(actual, upper)) ) return

#define TD_TEST_ASSERT_LESS(actual, lower)\
    if( !td::internal::test_assert_less(TDI_TEST_ARGS(actual, lower)) ) return

#define TD_TEST_ASSERT_LESS_OR_EQUAL(actual, lower)\
    if( !td::internal::test_assert_less_or_equal(TDI_TEST_ARGS(actual, lower)) ) return
