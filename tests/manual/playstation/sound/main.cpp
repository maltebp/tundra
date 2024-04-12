#include <psxgpu.h>

#include <tundra/input/playstation-input.hpp>
#include <tundra/gte/initialize.hpp>
#include <tundra/core/log.hpp>
#include <tundra/core/list.hpp>
#include <tundra/assets/sound/sound-asset.hpp>
#include <tundra/sound/playstation-sound.hpp>
#include <tundra/playstation-time.hpp>

namespace assets {

    extern "C" td::byte snd_jump[];
    extern "C" td::byte snd_drone_engine[];
    extern "C" td::byte snd_techno_music[];

}

int main() {

    TD_DEBUG_LOG("Initializer other stuff");
    
    ResetGraph(0);
    td::gte::initialize();

    td::PlayStationController controller_1;
    td::PlayStationController controller_2;
    td::playstation_input::initialize(); 
    td::playstation_input::update_controllers(controller_1, controller_2);

    td::PlayStationTime time;

    TD_DEBUG_LOG("Initializing sound");
    td::playstation_sound::initialize(&time);

    TD_DEBUG_LOG("Loading sounds");
    auto load_sound_asset = [](const td::byte* data, const td::String& name ) {
        const td::SoundAsset& asset = td::playstation_sound::load_sound(data);
        TD_DEBUG_LOG(
            "  Loaded %s: length = %ss, sample_rate = %d, looping = %s",
            name, asset.length, asset.sample_rate, asset.is_looping 
        );
        return asset;
    };
    const td::SoundAsset& jump_sound = load_sound_asset(assets::snd_jump, "jump");
    const td::SoundAsset& drone_engine_sound = load_sound_asset(assets::snd_drone_engine, "drone_engine");
    const td::SoundAsset& techno_music = load_sound_asset(assets::snd_techno_music, "techno_music");

    td::List<td::uint32> jump_sound_play_ids;
    td::List<td::uint32> drone_engine_sound_play_ids;
    
    TD_DEBUG_LOG("Starting loop");
    while( true ) {
        
        td::playstation_input::update_controllers(controller_1, controller_2);

        if( controller_1.is_pressed_this_frame(td::Button::Cross) ) {
            if( !controller_1.is_pressed(td::Button::L1)) {
                TD_DEBUG_LOG("Playing jump");
                td::uint32 play_id = td::playstation_sound::play_sound(jump_sound);
                jump_sound_play_ids.add(play_id);
            }
            else {
                TD_DEBUG_LOG("Stopping sounds sounds");
                for( td::uint32 i = 0; i < jump_sound_play_ids.get_size(); i++ ) {
                    td::playstation_sound::stop_sound(jump_sound_play_ids[i]);
                }
                jump_sound_play_ids.clear();
            }
        }

        if( controller_1.is_pressed_this_frame(td::Button::Square) ) {
            if( !controller_1.is_pressed(td::Button::L1)) {
                TD_DEBUG_LOG("Playing drone engine");
                td::uint32 play_id = td::playstation_sound::play_sound(drone_engine_sound);
                drone_engine_sound_play_ids.add(play_id);
            }
            else {
                TD_DEBUG_LOG("Stopping drone engine sounds");
                for( td::uint32 i = 0; i < drone_engine_sound_play_ids.get_size(); i++ ) {
                    td::playstation_sound::stop_sound(drone_engine_sound_play_ids[i]);
                }
                drone_engine_sound_play_ids.clear();
            }
            
        }

        if( controller_1.is_pressed_this_frame(td::Button::Circle) ) {
            
            if( !controller_1.is_pressed(td::Button::L1) ) {
                TD_DEBUG_LOG("Playing music");
                td::playstation_sound::play_music(techno_music);
            }
            else {
                TD_DEBUG_LOG("Stopping music");
                td::playstation_sound::stop_music();
            }
        }

        VSync(0); // Needed for input to update
    }

    return 0;
}