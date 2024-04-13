#pragma once

#include <tundra/frame-time.hpp>
#include <tundra/engine-settings.hpp>
#include <tundra/engine-systems.hpp>
#include <tundra/engine.hpp>

extern const td::EngineSettings ENGINE_SETTINGS;

extern void initialize(td::EngineSystems& engine_systems);

extern void update(td::EngineSystems& engine_systems, const td::FrameTime& frame_time);

int main() {
    
    td::Engine engine(&initialize, &update, ENGINE_SETTINGS);
    engine.run();


    return 0;
}