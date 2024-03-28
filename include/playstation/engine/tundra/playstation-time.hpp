#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/core/time.hpp>

namespace td {

    class PlayStationTime : ITime {
    public:

        // Constructing this will initialize the hardware time system
        PlayStationTime();

        virtual Duration get_duration_since_start() const override;

    private:

        uint64 initialization_time_microsecond;

    };

}