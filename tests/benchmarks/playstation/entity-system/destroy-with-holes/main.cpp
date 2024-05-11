#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "entity-system/base.hpp"
#include "tundra/core/list.dec.hpp"
#include "tundra/core/types.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

constexpr td::uint32 NUM_COMPONENTS = 1000;

td::List<td::internal::ComponentBase*> components;

void destroy_entities() {
    for( td::uint32 i = 0; i < components.get_size(); i++ ) {
        components[i]->destroy();
    }
}

template<typename TComponent>
td::Duration measure_construction(td::ITime& time) {

    td::List<td::Entity*> entities;
    td::List<TComponent*> components_to_destroy;

    for( td::uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
        td::Entity* e = td::Entity::create();
        components_to_destroy.add(e->add_component<TComponent>());
        components.add(e->add_component<TComponent>());
        components_to_destroy.add(e->add_component<TComponent>());
        entities.add(e);
    }

    for( td::uint32 i = 0; i < components_to_destroy.get_size(); i++ ) {
        components_to_destroy[i]->destroy();
    }

    td::Duration duration = measure(time, destroy_entities);
    components.clear();

    for( td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->destroy();
    }
    entities.clear();

    return duration;
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