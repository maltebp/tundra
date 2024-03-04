#include "tundra/test-framework/test-runner.hpp"

#include <cstdio>

#include "tundra/test-framework/test.hpp"

namespace td {

    int TestRunner::run() {

        std::printf("\nTests to run: %d\n\n", Test::num_tests);

        bool any_tests_failed = false;
        
        for( int i = 0; i < Test::num_tests; i++ ) {
            Test* test = Test::tests[i];

            Test::current_failure_report = {};
            Test::last_test_failed = false;
            
            std::printf("  %s..", test->name);

            test->function(test);
            
            if( Test::last_test_failed ) {
                any_tests_failed = true;
                std::fprintf(stderr, " FAILURE\n", test->name);
                std::fprintf(stderr, "      %s\n", Test::current_failure_report.comparison_string);
                std::fprintf(stderr, "       in %s:%d\n", Test::current_failure_report.file_name, Test::current_failure_report.line_number);
            }
            else {
                std::printf(" Success\n");
            }
        }

        if( any_tests_failed ) {
            std::fprintf(stderr, "\nTESTS FAILED!\n");
        }
        else {
            std::printf("\nAll tests succeeded\n");
        }
        
        return any_tests_failed ? -1 : 0;
    }
}