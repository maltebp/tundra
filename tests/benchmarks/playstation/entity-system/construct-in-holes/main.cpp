#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "entity-system/base.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

constexpr td::uint32 NUM_COMPONENTS = 1000;

td::List<td::Entity*> entities;
td::List<td::Entity*> non_measured_entities;

template<typename TComponent>
void create_components() {
    for( td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->add_component<TComponent>();
    }
}

template<typename TComponent>
td::Duration measure_construction(td::ITime& time) {
    for(td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->destroy();
    }
    entities.clear();

    for( td::uint32 i = 0 ; i < non_measured_entities.get_size(); i++ ) {
        non_measured_entities[i]->destroy();
    }
    non_measured_entities.clear();

    td::List<TComponent*> components_to_delete;
    
    for( td::uint32 i = 0; i < NUM_COMPONENTS * 2; i++ ) {
        td::Entity* e = td::Entity::create();
        non_measured_entities.add(e);
        e->add_component<TComponent>();
        components_to_delete.add(e->add_component<TComponent>());
    }

    for( td::uint32 i = 0; i < components_to_delete.get_size(); i++ ) {
        components_to_delete[i]->destroy();
    }

    for( td::uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
        entities.add(td::Entity::create());
    }

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