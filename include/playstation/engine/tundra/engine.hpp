#pragma once

#include <tundra/frame-time.hpp>
#include <tundra/engine-settings.hpp>
#include <tundra/engine-systems.hpp>

namespace td {

    class Engine {
    public:

        using InitializeCallback = void (*)(EngineSystems& engine_systems);
        using UpdateCallback = void (*)(EngineSystems& engine_systems, const FrameTime& frame_time);

        Engine(InitializeCallback initialize_callback, UpdateCallback update_callback, EngineSettings settings);

        void run();        

    private:

        const InitializeCallback initialize_callback;
        const UpdateCallback update_callback;
        const EngineSettings settings;

    };

}