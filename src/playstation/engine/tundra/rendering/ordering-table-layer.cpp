#include <tundra/rendering/ordering-table-layer.hpp>

#include <psxgpu.h>

#include <tundra/rendering/ordering-table-node.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.hpp>
#include <tundra/core/assert.hpp>

namespace td {

    OrderingTableLayer::OrderingTableLayer(uint16 resolution, UFixed16<12> far_plane) 
        :   resolution(resolution),
            far_plane(far_plane),
            z_map_factor_3(UFixed32<12>{resolution} / (UFixed32<12>{far_plane} * 3)),
            z_map_factor_4(UFixed32<12>{resolution} / (UFixed32<12>{far_plane} * 4)),
            ordering_table(resolution)
    {
        TD_ASSERT(resolution > 0, "Resolution must be larger than 0");
        TD_ASSERT(far_plane > 0, "Far plane must be larger than 0");
    }

    void OrderingTableLayer::add_node(OrderingTableNode* node, uint16 resolution_index) {
        TD_ASSERT(
            resolution_index < resolution, 
            "Resolution index was out of bounds (was %d, but resolution is %d)", 
            resolution_index, resolution
        );

        OrderingTableNode& node_to_append_to = ordering_table[resolution_index];
        node->next_node_ptr = node_to_append_to.next_node_ptr;
        node_to_append_to.set_next_node(node);

        if( &node_to_append_to == front_node ) {
            front_node = node;
        }

        num_added_nodes++;
    }

    void OrderingTableLayer::add_to_front(OrderingTableNode* node) {
        node->next_node_ptr = front_node->next_node_ptr;
        front_node->set_next_node(node);
        front_node = node;
        num_added_nodes++;
    }

    uint16 OrderingTableLayer::get_resolution() const { 
        return resolution;
    }

    uint32 OrderingTableLayer::get_z_map_factor_3() const { 
        return z_map_factor_3;
    }

    uint32 OrderingTableLayer::get_z_map_factor_4() const { 
        return z_map_factor_4;
    }
}