#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "general.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {

    constexpr td::uint32 NUM_ENTITIES = 1000;
    
    for( td::uint32 i = 0; i < NUM_ENTITIES; i++ ) {
        td::Entity* e = td::Entity::create();
        e->add_component<SmallComponent>();
        e->add_component<MediumComponent>();
        e->add_component<LargeComponent>();
    }

    td::Duration duration_small = measure(engine_systems.time, iterate_components<SmallComponent>);
    td::Duration duration_medium = measure(engine_systems.time, iterate_components<MediumComponent>);
    td::Duration duration_big = measure(engine_systems.time, iterate_components<LargeComponent>);

    std::printf("Small, Medium, Big\n");
    
    std::printf("%s, %s, %s\n", 
        td::to_string(duration_small.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_medium.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_big.to_milliseconds(), 4).get_c_string()
    );

    engine_systems.exit_requested = true;

    pcsx_exit(0);
}