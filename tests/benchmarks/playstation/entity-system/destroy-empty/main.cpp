#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "entity-system/base.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

constexpr td::uint32 NUM_COMPONENTS = 1000;

td::List<td::internal::ComponentBase*> components_to_destroy;

template<typename TComponent>
void destroy_entity() {
    for( td::uint32 i = 0; i < components_to_destroy.get_size(); i++ ) {
        components_to_destroy[i]->destroy();
    }
}

template<typename TComponent>
td::Duration measure_construction(td::ITime& time) {
    td::List<td::Entity*> entities;

    for( td::uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
        td::Entity* e = td::Entity::create();
        TComponent* component = e->add_component<TComponent>();
        entities.add(e);
        components_to_destroy.add(component);
    }

    td::Duration duration = measure(time, destroy_entity<TComponent>);
    components_to_destroy.clear();

    for(td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->destroy();
    }

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