#include <tundra/startup.hpp>

#include <cstdlib>
#include <tundra/engine/entity-system/entity.hpp>

#include "base.hpp"
#include "transforms/base.hpp"
#include "tundra/core/duration.hpp"
#include "tundra/core/fixed.hpp"
#include "tundra/core/list.dec.hpp"
#include "tundra/engine/dynamic-transform.hpp"

const td::EngineSettings ENGINE_SETTINGS { 30000 };

extern void initialize(td::EngineSystems&) { }

void construct_top_down_recursion(
    const td::List<td::uint32>& hierarchy, 
    td::DynamicTransform* parent, 
    td::uint32 current_depth
) {
    if( current_depth >= hierarchy.get_size() ) return;

    td::uint32 num_children_to_add = hierarchy[current_depth];

    for( td::uint32 i = 0; i < num_children_to_add; i++ ) {
        td::Entity* entity = td::Entity::create();

        td::DynamicTransform* child = entity->add_component<td::DynamicTransform>();
        parent->add_child(child);

        construct_top_down_recursion(hierarchy, child, current_depth + 1);
    }
}

td::Duration destroy_top_down(td::ITime& time, const td::List<td::uint32>& hierarchy) {

    print_hierarchy(hierarchy);

    td::List<td::DynamicTransform*> roots;

    for( td::uint32 i = 0; i < 240; i++) {

        td::Entity* entity = td::Entity::create();

        td::DynamicTransform* root = entity->add_component<td::DynamicTransform>();
        roots.add(root);

        construct_top_down_recursion(hierarchy, root, 0);    

        std::HeapUsage heap_usage;
        std::GetHeapUsage(&heap_usage);
        std::printf("Memory: %d (max = %d)\n", heap_usage.alloc, heap_usage.alloc_max);
    }

    // The measured destruction
    td::Duration start = time.get_duration_since_start();
    for( td::uint32 i = 0; i < roots.get_size(); i++ ) {
        roots[i]->add_rotation({ td::to_fixed(0.5), td::to_fixed(0.5), td::to_fixed(0.5) });
        roots[i]->add_scale({ 1, 1, 1 });
        roots[i]->add_translation({ 1, 1, 1 });
    }
    td::Duration duration = time.get_duration_since_start() - start;
    
    td::uint32 num_entities_destroyed = 0;
    for( td::Entity* entity : td::Entity::get_all() ) {
        entity->destroy();
        num_entities_destroyed++;
    }
    std::printf("Num entities destroyed: %u\n", num_entities_destroyed);

    std::printf("Entities: %u / %u\n",
        td::internal::Registry<td::Entity>::get_num_allocated_components(),
        td::internal::Registry<td::Entity>::get_num_blocks() * td::internal::Registry<td::Entity>::BLOCK_SIZE
    );   

    std::printf("Transforms: %u / %u\n",
        td::internal::Registry<td::DynamicTransform>::get_num_allocated_components(),
        td::internal::Registry<td::DynamicTransform>::get_num_blocks() * td::internal::Registry<td::DynamicTransform>::BLOCK_SIZE
    );   

    return duration;
}

extern void update(td::EngineSystems& engine_systems, const td::FrameTime&) {
    
    td::Duration duration_small = destroy_top_down(engine_systems.time, HIERARCHY_SMALL);
    td::Duration duration_medium = destroy_top_down(engine_systems.time, HIERARCHY_MEDIUM);
    td::Duration duration_large = destroy_top_down(engine_systems.time, HIERARCHY_LARGE);
    td::Duration duration_wide = destroy_top_down(engine_systems.time, HIERARCHY_WIDE);

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