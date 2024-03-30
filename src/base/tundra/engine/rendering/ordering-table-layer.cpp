#include <tundra/engine/rendering/ordering-table-layer.hpp>

#include <psxgpu.h>

#include <tundra/core/list.hpp>
#include <tundra/core/assert.hpp>

namespace td {

    OrderingTableLayer::OrderingTableLayer() : OrderingTableLayer(0, 0, 1) { }

    OrderingTableLayer::OrderingTableLayer(Fixed32<12> near_plane, Fixed32<12> far_plane, const List<OrderingTableLayerPart>& parts) 
        :   near_plane(near_plane),
            far_plane(near_plane + parts.get_last().far_plane),
            parts(parts),
            ordering_table(compute_total_resolution(parts))
    {
        TD_ASSERT(parts.get_size() > 0, "Parts must not be empty");
        TD_ASSERT(far_plane > near_plane, "Far plane must be larger than near plane (far plane was %s, near plane was %s)", far_plane, near_plane);
    }

    OrderingTableLayer::OrderingTableLayer(Fixed32<12> near_plane, Fixed32<12> far_plane, uint16 resolution) 
        :   near_plane(near_plane),
            far_plane(far_plane),
            ordering_table(resolution)
    { 
        TD_ASSERT(resolution != 0, "Depth resolution must not be 0");
        TD_ASSERT(far_plane > near_plane, "Far plane must be larger than near plane (far plane was %s, near plane was %s)", far_plane, near_plane);

        parts.add({far_plane, resolution});
    }

    void OrderingTableLayer::add_node(OrderingTableNode* node, td::Fixed32<12> z) {
        if( z < near_plane ) return;

        Fixed32<12> part_near_plane = near_plane;
        uint32 part_node_index = 0;

        for( uint32 i = 0; i < parts.get_size(); i++ ) {
            if( z >= parts[i].far_plane ) {
                part_near_plane = parts[i].far_plane;
                part_node_index += parts[i].resolution;
            }
            else {
                Fixed32<12> local_z_signed = z - part_near_plane;
                TD_ASSERT(local_z_signed >= 0, "Internal error: node's local z %s was less than 0 ", local_z_signed);
                TD_ASSERT(local_z_signed < (parts[i].far_plane - part_near_plane), "Internal error: node's local z %s was not below local far plane %s", local_z_signed, far_plane);
                UFixed32<12> local_z_unsigned{ local_z_signed };
                uint32 node_index = part_node_index + (uint32)(local_z_unsigned / parts[i].resolution);  
                addPrim(&parts[node_index], node);
            }
        }
    }

    [[nodiscard]] uint16 OrderingTableLayer::compute_total_resolution(const List<OrderingTableLayerPart>& parts) {
        uint32 total_resolution = 0;
        for( uint32 i = 0; i < parts.get_size(); i++ ) {
            total_resolution += parts[i].resolution;
        }
        
        TD_ASSERT(
            total_resolution, 
            "OrderingTableLayer's total resolution is larger than the max of %d (was %d)",
            td::limits::numeric_limits<uint16>::max,
            total_resolution
        );

        return (uint16) total_resolution;
    }

}