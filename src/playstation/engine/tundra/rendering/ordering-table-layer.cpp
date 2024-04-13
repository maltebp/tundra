#include <tundra/rendering/ordering-table-layer.hpp>

#include <psxgpu.h>

#include <tundra/rendering/ordering-table-node.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.hpp>
#include <tundra/core/assert.hpp>

namespace td {

    OrderingTableLayer::OrderingTableLayer(OrderingTableLayerSettings settings) 
        :   OrderingTableLayer(settings.resolution, settings.far_plane)
    { }

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

    void OrderingTableLayer::add_font_to_front(PrimitiveBuffer& primitive_buffer, int32 x, int32 y, const td::String& text) {
        
        uint32 node_after_front_address = front_node->next_node_ptr;
        
        // TODO: This approach will not work once the primitive buffer allocates buffer
        // memory dynamically, and thus may not allocate this in sequence
        byte* allocation_ptr = (byte*)primitive_buffer.allocate(1);
        
        byte* allocation_end = (byte*)FntSort(
            reinterpret_cast<uint32*>(front_node), 
            allocation_ptr, 
            x, y,
            text.get_c_string());

        uint32 allocated_size = (uint32)((byte*)allocation_ptr - allocation_end);
        TD_ASSERT(allocated_size > 0, "FntSort allocated no primitives");
        
        void* allocated_ptr = primitive_buffer.allocate(allocated_size - 1);
        TD_ASSERT(
            allocated_ptr != nullptr,
            "Primitive buffer ran out of memory while writing font (memory has been corrupted) - increase primitive buffer size");

        // Find the new front node by traversing text nodes
        OrderingTableNode* current_node = front_node;
        while( current_node->next_node_ptr != node_after_front_address  ) {
            current_node = reinterpret_cast<OrderingTableNode*>((uint32)current_node->next_node_ptr);
            TD_ASSERT(current_node->next_node_ptr != 0, "Text OT node points to nullptr"); 
        }        

        front_node = current_node;
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