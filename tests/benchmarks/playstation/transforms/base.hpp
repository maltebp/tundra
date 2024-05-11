#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/list.hpp>

struct Hierarchy {
    td::uint32 levels;
    td::uint32 branching_factor;
};

constexpr td::uint32 NUM_TRANSFORMS = 240; // This is divisable with all hierarchies

static td::uint32 compute_num_total_elements(const td::List<td::uint32>& hierachy) {
    td::uint32 total = 1;
    td::uint32 previous_layer = 1;
    for( td::uint32 i = 0; i < hierachy.get_size(); i++ ) {
        td::uint32 this_layer = previous_layer * hierachy[i];
        total += this_layer;
        previous_layer = this_layer;
    }

    return total;
}

static void print_hierarchy(const td::List<td::uint32>& hierarchy) {
    td::uint32 hierarchy_size = compute_num_total_elements(hierarchy);
    td::uint32 num_entities = NUM_TRANSFORMS / hierarchy_size;

    std::printf("Hierarchy: 1");
    for( td::uint32 i = 0; i < hierarchy.get_size(); i++ ) {
        std::printf(", %u", hierarchy[i]);
    }
    std::printf(" (total = %u, entitites = %u)\n", hierarchy_size, num_entities);
}

// Each element represents the branching factor of layer
const td::List<td::uint32> HIERARCHY_SMALL { 1 };
const td::List<td::uint32> HIERARCHY_MEDIUM { 3, 2 };
const td::List<td::uint32> HIERARCHY_WIDE { 29 };
const td::List<td::uint32> HIERARCHY_LARGE { 3, 3, 3 };