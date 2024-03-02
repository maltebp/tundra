#pragma once

#include <tundra/core/types.hpp>

namespace td {

    class Test;

    class TestFailureReport {
    public:

        TestFailureReport() = default;

        TestFailureReport(Test* test, const char* comparison_string, const char* file_name, uint16 line_number)
            :   test(test), comparison_string(comparison_string), file_name(file_name), line_number(line_number)
        { }

        Test* test = nullptr;

        const char* comparison_string = nullptr;
        
        const char* file_name = nullptr;

        uint16 line_number = 0;

    };

}