#pragma once

#include <tundra/core/time.hpp>

[[maybe_unused]] static td::Duration measure(td::ITime& time, void (*function_to_measure)()) {
    td::Duration start = time.get_duration_since_start();
    function_to_measure();
    td::Duration end = time.get_duration_since_start();
    return end - start;
}

static __inline__ void pcsx_exit(int16_t code) { *((volatile int16_t* )0x1f802082) = code; }
