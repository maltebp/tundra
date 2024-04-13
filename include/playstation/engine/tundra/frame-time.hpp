#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/fixed.hpp>

namespace td {

    struct FrameTime {
        
        uint64 microseconds;

        Fixed32<12> milliseconds;

        Fixed32<12> seconds;

    };

}