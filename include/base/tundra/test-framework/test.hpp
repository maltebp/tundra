#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/macro-util.hpp>
#include <tundra/test-framework/internal/test-failure-report.hpp>


namespace td {

    class TestRunner;

    class Test;

    using TestFunction = void(*)(Test* td_internal_input_test);
    
    class Test {
        friend class TestRunner;
    public:
        
        Test(const char* name, TestFunction function);
        
        const char* const name;

        const TestFunction function;

        static TestFailureReport current_failure_report;

        static bool last_test_failed;

    private:

        static Test* tests[];

        static uint16 num_tests;

    };

}

#define TDI_TEST_GEN(name, internal_name)\
    static void TD_CONCAT(internal_name, _func)(td::Test*);\
    static td::Test internal_name{name, &TD_CONCAT(internal_name, _func)};\
    static void TD_CONCAT(internal_name, _func)(td::Test* td_internal_input_test)

#define TD_TEST(name) TDI_TEST_GEN(name, TD_CONCAT(td_test_, __COUNTER__))
