#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "entity-system/base.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {

    constexpr td::uint32 NUM_ENTITIES = 1000;
    constexpr td::uint32 HOLE_SIZE = 10;

    td::List<SmallComponent*> small_components_to_delete;
    td::List<MediumComponent*> medium_components_to_delete;
    td::List<LargeComponent*> large_components_to_delete;
    
    for( td::uint32 i = 0, created_entities = 0; created_entities < NUM_ENTITIES; i++ ) {
        td::Entity* e = td::Entity::create();
        if( (i / HOLE_SIZE) % 2 == 0 ) {
            created_entities++;
            e->add_component<SmallComponent>();
            e->add_component<MediumComponent>();
            e->add_component<LargeComponent>();
        }
        else {
            small_components_to_delete.add(e->add_component<SmallComponent>());
            medium_components_to_delete.add(e->add_component<MediumComponent>());
            large_components_to_delete.add(e->add_component<LargeComponent>());
        }
    }

    for( td::uint32 i = 0; i < small_components_to_delete.get_size(); i++ ) {
        small_components_to_delete[i]->destroy();
    }

    for( td::uint32 i = 0; i < medium_components_to_delete.get_size(); i++ ) {
        medium_components_to_delete[i]->destroy();
    }

    for( td::uint32 i = 0; i < large_components_to_delete.get_size(); i++ ) {
        large_components_to_delete[i]->destroy();
    }

    td::Duration duration_small = measure(engine_systems.time, iterate_components<SmallComponent>);
    td::Duration duration_medium = measure(engine_systems.time, iterate_components<MediumComponent>);
    td::Duration duration_large = measure(engine_systems.time, iterate_components<LargeComponent>);

    std::printf("Small, Medium, Large\n");
    
    std::printf("%s, %s, %s\n", 
        td::to_string(duration_small.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_medium.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_large.to_milliseconds(), 4).get_c_string());

    engine_systems.exit_requested = true;

    pcsx_exit(0);
}