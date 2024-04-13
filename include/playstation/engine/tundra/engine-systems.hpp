#pragma once

#include <tundra/core/time.hpp>
#include <tundra/input/input.hpp>
#include <tundra/sound/i-sound-player.hpp>
#include <tundra/assets/asset-loader.hpp>

namespace td {

    class RenderSystem;

    struct EngineSystems {
    public:

        AssetLoader& asset_load;
        ITime& time;
        Input& input;
        ISoundPlayer& sound_player;
        RenderSystem& render;

    };

}