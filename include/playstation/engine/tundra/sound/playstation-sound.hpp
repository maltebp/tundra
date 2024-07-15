#pragma once

#include <tundra/core/time.hpp>

#include <tundra/assets/sound/sound-asset.hpp>


namespace td::playstation_sound {

    void initialize(const ITime* time);

    [[nodiscard]] const SoundAsset& load_sound(const td::byte* data);

    // Returns the ID of the sound
    uint32 play_sound(const SoundAsset& sound);

    void play_music(const SoundAsset& music);

    void stop_sound(uint32 sound_play_id);

    void stop_music();

    uint32 get_sram_used();

}