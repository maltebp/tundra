#pragma once

#include <tundra/sound/i-sound-player.hpp>
#include <tundra/sound/playstation-sound.hpp>

namespace td {

    class PlayStationSoundPlayer : public ISoundPlayer {
    public:

        // Returns the play ID of the sound
        virtual uint32 play_sound(const SoundAsset& sound) override {
            return playstation_sound::play_sound(sound);
        }

        virtual void play_music(const SoundAsset& music) override {
            playstation_sound::play_music(music);
        }

        virtual void stop_sound(uint32 sound_play_id) override {
            playstation_sound::stop_sound(sound_play_id);
        }

        virtual void stop_music() override {
            playstation_sound::stop_music();
        }

    };

}