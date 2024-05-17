#include <tundra/engine.hpp>

#include <psxgte.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>

#include <tundra/frame-time.hpp>
#include <tundra/assets/asset-loader.hpp>
#include <tundra/gte/initialize.hpp>
#include <tundra/engine-systems.hpp>
#include <tundra/rendering/vram-allocator.hpp>
#include <tundra/rendering/vram-allocator.hpp>
#include <tundra/rendering/primitive-buffer.hpp>
#include <tundra/rendering/render-system.hpp>
#include <tundra/input/input.hpp>
#include <tundra/input/playstation-controller.hpp>
#include <tundra/input/playstation-input.hpp>
#include <tundra/playstation-time.hpp>
#include <tundra/sound/playstation-sound-player.hpp>
#include <tundra/sound/playstation-sound.hpp>

namespace td {

    Engine::Engine(
        InitializeCallback initialize_callback, 
        UpdateCallback update_callback, 
        EngineSettings settings
    ) 
        :   initialize_callback(initialize_callback),
            update_callback(update_callback),
            settings(settings)
    { }

    void Engine::run() {

        ResetGraph(0);

        td::gte::initialize();

        // Set the origin of screen space
        gte_SetGeomOffset(320 / 2, 240/ 2);
        
        // Set screen depth (basically FOV)
        gte_SetGeomScreen(320 / 2);

        PlayStationTime time;

        playstation_sound::initialize(&time);
        PlayStationSoundPlayer sound_player;

        playstation_input::initialize();
        PlayStationController controller_1;
        PlayStationController controller_2;
        playstation_input::update_controllers(controller_1, controller_2);
        Input input { controller_1, controller_2 };

        VramAllocator vram_allocator;
        RenderSystem render_system{ time, vram_allocator, settings.primitive_buffer_size };   

        AssetLoader asset_loader{vram_allocator};   

        EngineSystems systems {  asset_loader, time, input, sound_player,  render_system };    

        initialize_callback(systems);

        bool is_first_loop = true;  

        Duration last_frame_time = time.get_duration_since_start();
        FrameTime frame_time;

        while( true ) {

            Duration this_frame_time = time.get_duration_since_start();
            Duration time_since_last_frame = this_frame_time - last_frame_time;
            frame_time.microseconds = (uint64)time_since_last_frame.microseconds;
            frame_time.seconds = time_since_last_frame.to_seconds();
            frame_time.milliseconds = time_since_last_frame.to_milliseconds();
            last_frame_time = this_frame_time;

            td::playstation_input::update_controllers(controller_1, controller_2);

            update_callback(systems, frame_time);
            
            render_system.render();

            if( is_first_loop ) {
                is_first_loop = false;
                SetDispMask(1); 
            }
        }
    }

}