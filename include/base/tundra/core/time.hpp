#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/duration.hpp>

namespace td {

    // Low-level utility for providing the clock
    class ITime {
    public:

        virtual Duration get_duration_since_start() const = 0;

    };

}