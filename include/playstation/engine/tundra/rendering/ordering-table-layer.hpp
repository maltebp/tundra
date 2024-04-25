#pragma once

#include "tundra/core/types.hpp"
#include "tundra/rendering/primitive-buffer.hpp"
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.dec.hpp>

#include <tundra/rendering/ordering-table-node.hpp>


namespace td {

    class OrderingTable;

    struct OrderingTableLayerSettings {
        uint16 resolution;
        UFixed16<12> far_plane;
    };

    class OrderingTableLayer {
    public:

        OrderingTableLayer(OrderingTableLayerSettings settings);

        OrderingTableLayer(
            uint16 resolution, 
            UFixed16<12> far_plane = td::limits::numeric_limits<UFixed16<12>>::max
        );

        void add_node(OrderingTableNode* node, uint16 resolution_index);

        void add_to_front(OrderingTableNode* node);

        // This function is a bit misplaced, but it is because of the rather
        // annoying API of FntSort.
        void add_font_to_front(PrimitiveBuffer& primitive_buffer, int32 x, int32 y, const td::String& text);

        [[nodiscard]] uint16 get_resolution() const;

        // The Z factor that the sum of 3 z values are multiplied with to map it the ordering table
        [[nodiscard]] uint16 get_z_map_factor_3() const;
        
        // The Z factor that the sum of 4 z values are multiplied with to map it the ordering table
        [[nodiscard]] uint16 get_z_map_factor_4() const;

    private:

        const uint16 resolution;
        const UFixed16<12> far_plane;
        const uint16 z_map_factor_3;
        const uint16 z_map_factor_4;
        
        uint32 num_added_nodes = 0;
        OrderingTableNode* front_node;

        List<OrderingTableNode> ordering_table;

        friend class OrderingTable;

    };

}