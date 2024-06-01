#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "entity-system/base.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

constexpr td::uint32 NUM_COMPONENTS = 1;
constexpr td::uint32 NUM_SIBLINGS = 0;

td::List<td::Entity*> entities;

template<typename TComponent>
void create_components() {
    for( td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->add_component<TComponent>();
    }
}

template<typename TComponent>
td::Duration measure_construction(td::ITime& time) {
    TComponent::reserve(5000);

    for(td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->destroy();
    }

    entities.clear();

    for( td::uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
        td::Entity* e = td::Entity::create();
        entities.add(e);
        for( td::uint32 j = 0; j < NUM_SIBLINGS; j++ ) {
            e->add_component<TComponent>();
        }
    }

    // td::Entity* dummy = td::Entity::create();
    // dummy->add_component<TComponent>();

    return measure(time, create_components<TComponent>);
}

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {

    td::Duration duration_small = measure_construction<SmallComponent>(engine_systems.time);
    td::Duration duration_medium = measure_construction<MediumComponent>(engine_systems.time);
    td::Duration duration_large = measure_construction<LargeComponent>(engine_systems.time);

    std::printf("Small, Medium, Large\n");
    
    std::printf("%s, %s, %s\n", 
        td::to_string(duration_small.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_medium.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_large.to_milliseconds(), 4).get_c_string()
    );

    engine_systems.exit_requested = true;

    pcsx_exit(0);
}