#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/test.hpp>

#include <tundra/core/string.hpp>
#include <tundra/playstation-time.hpp>

template<>
td::String td::to_string<td::Duration>(const Duration& duration) {
    // PlayStation does not support printing '%lld' format, so we make do with
    // int32 ('%d' format) instead
    return to_string<int32>((int32)duration.microseconds);
}

namespace td::playstation_time_tests {

    TD_TEST("playstation-time") {
        
        PlayStationTime time;
        
        Duration start;
        while( true ) {
            Duration duration_since_start = time.get_duration_since_start();
            if( duration_since_start.to_milliseconds() >= to_fixed(10) ) {
                start = duration_since_start;
                break;
            }
        }

        TD_TEST_ASSERT_GREATER(start, Duration{0});

        Duration end;
        while( true ) {
            end = time.get_duration_since_start();
            Duration difference = end - start;
            if( difference.to_seconds() >= to_fixed(0.1) ) {
                break;
            }
        }

        TD_TEST_ASSERT_GREATER(end, start);
    }
}