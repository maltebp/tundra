#include <tundra/test-framework/test.hpp>

#include <tundra/core/assert.hpp>

namespace td {

    const uint32 MAX_NUM_TESTS = 1000;

    Test* Test::tests[MAX_NUM_TESTS] = {0};

    uint16 Test::num_tests = 0;

    TestFailureReport Test::current_failure_report{};

    bool Test::last_test_failed = false;

    Test::Test(const char* name, TestFunction function) 
        :   name(name), function(function)
    { 
        // TODO: More validation on name
        // TD_ASSERT(name != "", "Test name must not be empty");
        
        *(tests + num_tests) = this;
        num_tests++;
    }

}