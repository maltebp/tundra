#pragma once

#include <tundra/core/fixed.hpp>

#include <tundra/assets/sound/sound-asset-load-info.fwd.hpp>

namespace td {

    class SoundAsset {
    public:

        // Sample rate in integer (e.g. 44500 Hz)
        uint32 sample_rate;

        // Length in seconds
        UFixed32<12> length;

        bool is_looping;

        SoundAssetLoadInfo* load_info;        

    };

}