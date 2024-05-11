#include <tundra/startup.hpp>

#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "transforms/base.hpp"
#include "tundra/core/duration.hpp"
#include "tundra/core/list.dec.hpp"
#include "tundra/engine/dynamic-transform.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

void construct_top_down_recursion(td::ITime& time, const td::List<td::uint32>& hierarchy, td::Entity* entity, td::DynamicTransform* parent, td::uint32 current_depth, td::List<td::DynamicTransform*>& transforms) {
    if( current_depth >= hierarchy.get_size() ) return;

    td::uint32 num_children_to_add = hierarchy[current_depth];
    
    td::List<td::DynamicTransform*> children;
    children.reserve(num_children_to_add);

    for( td::uint32 i = 0; i < num_children_to_add; i++ ) {
        td::DynamicTransform* child = entity->add_component<td::DynamicTransform>();
        parent->add_child(child);
        transforms.add(child);
        construct_top_down_recursion(time, hierarchy, entity, child, current_depth + 1, transforms);
    }
}

td::Duration construct_top_down(td::ITime& time, const td::List<td::uint32>& hierarchy) {

    td::uint32 hierarchy_size = compute_num_total_elements(hierarchy);
    td::uint32 num_entities = 240 / hierarchy_size;
    td::List<td::Entity*> entities;
    entities.reserve(num_entities);

    td::Duration total_duration = 0;

    td::List<td::DynamicTransform*> transforms; 

    for( td::uint32 i = 0; i < num_entities; i++) {
        td::Entity* entity = td::Entity::create();
        entities.add(entity);

        td::Duration start = time.get_duration_since_start();
        td::DynamicTransform* root = entity->add_component<td::DynamicTransform>();
        transforms.add(root);

        construct_top_down_recursion(time, hierarchy, entity, root, 0, transforms);    
        total_duration += time.get_duration_since_start() - start;
    }
    
    for( td::uint32 i = 0; i < entities.get_size(); i++ ) {
        entities[i]->destroy();
    }

    return total_duration;
}

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {
    
    td::Duration duration_small = construct_top_down(engine_systems.time, HIERARCHY_SMALL);
    td::Duration duration_medium = construct_top_down(engine_systems.time, HIERARCHY_MEDIUM);
    td::Duration duration_large = construct_top_down(engine_systems.time, HIERARCHY_LARGE);
    td::Duration duration_wide = construct_top_down(engine_systems.time, HIERARCHY_WIDE);

    std::printf("Small, Medium, Large, Wide\n");
    
    std::printf("%s, %s, %s, %s\n", 
        td::to_string(duration_small.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_medium.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_large.to_milliseconds(), 4).get_c_string(),
        td::to_string(duration_wide.to_milliseconds(), 4).get_c_string()
    );

    engine_systems.exit_requested = true;

    pcsx_exit(0);
}