#include <tundra/startup.hpp>

#include <cstdlib>
#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "transforms/base.hpp"
#include "tundra/core/duration.hpp"
#include "tundra/core/fixed.hpp"
#include "tundra/core/list.dec.hpp"
#include "tundra/engine/dynamic-transform.hpp"
#include "tundra/engine/transform-matrix.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

void construct_top_down_recursion(
    const td::List<td::uint32>& hierarchy, 
    td::DynamicTransform* parent, 
    td::uint32 current_depth,
    td::List<td::DynamicTransform*>& transforms_from_top_to_bottom
) {
    if( current_depth >= hierarchy.get_size() ) return;

    td::uint32 num_children_to_add = hierarchy[current_depth];

    for( td::uint32 i = 0; i < num_children_to_add; i++ ) {
        td::Entity* entity = td::Entity::create();

        td::DynamicTransform* child = entity->add_component<td::DynamicTransform>();
        child->add_rotation({ td::to_fixed(0.1), td::to_fixed(0.1), td::to_fixed(0.1) });
        child->add_scale({ td::to_fixed(0.125), td::to_fixed(0.125), td::to_fixed(0.125) });
        child->add_translation({ 1, 1, 1 });

        parent->add_child(child);

        construct_top_down_recursion(hierarchy, child, current_depth + 1, transforms_from_top_to_bottom);
        transforms_from_top_to_bottom.add(child);
    }
}

td::Duration compute_bottom_up(td::ITime& time, const td::List<td::uint32>& hierarchy) {

    print_hierarchy(hierarchy);

    td::uint32 hierarchy_size = compute_num_total_elements(hierarchy);
    td::uint32 num_hierarchies = NUM_TRANSFORMS / hierarchy_size;

    td::List<td::DynamicTransform*> transforms_from_bottom_to_top;

    for( td::uint32 i = 0; i < num_hierarchies; i++) {

        td::Entity* entity = td::Entity::create();

        td::DynamicTransform* root = entity->add_component<td::DynamicTransform>();
        root->add_rotation({ td::to_fixed(0.1), td::to_fixed(0.1), td::to_fixed(0.1) });
        root->add_scale({ td::to_fixed(0.125), td::to_fixed(0.125), td::to_fixed(0.125) });
        root->add_translation({ 1, 1, 1 });

        construct_top_down_recursion(hierarchy, root, 0, transforms_from_bottom_to_top);    
        transforms_from_bottom_to_top.add(root);

        std::HeapUsage heap_usage;
        std::GetHeapUsage(&heap_usage);
    }

    // The measured destruction
    td::Duration start = time.get_duration_since_start();
    for( td::uint32 i = 0; i < transforms_from_bottom_to_top.get_size(); i++ ) {
        td::gte::compute_world_matrix(transforms_from_bottom_to_top[i]);
    }
    td::Duration duration = time.get_duration_since_start() - start;
    
    for( td::Entity* entity : td::Entity::get_all() ) {
        entity->destroy();
    }

    return duration;
}

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {
    
    td::Duration duration_small = compute_bottom_up(engine_systems.time, HIERARCHY_SMALL);
    td::Duration duration_medium = compute_bottom_up(engine_systems.time, HIERARCHY_MEDIUM);
    td::Duration duration_large = compute_bottom_up(engine_systems.time, HIERARCHY_LARGE);
    td::Duration duration_wide = compute_bottom_up(engine_systems.time, HIERARCHY_WIDE);

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