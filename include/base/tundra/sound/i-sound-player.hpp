#pragma once

#include <tundra/core/types.hpp>
#include <tundra/assets/sound/sound-asset.hpp>

namespace td {

    class ISoundPlayer {
    public:

        // Returns the play ID of the sound
        virtual uint32 play_sound(const SoundAsset& sound) = 0;

        virtual void play_music(const SoundAsset& music) = 0;

        virtual void stop_sound(uint32 sound_play_id) = 0;

        virtual void stop_music() = 0;

    };

}