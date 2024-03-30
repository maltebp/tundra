#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/core/list.dec.hpp>

#include <tundra/engine/rendering/ordering-table-node.hpp>


namespace td {

    class OrderingTable;

    struct OrderingTableLayerPart {
        Fixed32<12> far_plane;
        uint16 resolution;
    };

    class OrderingTableLayer {
    public:

        // Creates a layer with a single level with no far/near-plane limits
        // (i.e. primitives are rendered in order they are submitted)
        // TODO: Actually, I think they are rendered in reverse order - check this!
        OrderingTableLayer();

        OrderingTableLayer(Fixed32<12> near_plane, Fixed32<12> far_plane, const List<OrderingTableLayerPart>& parts);

        OrderingTableLayer(Fixed32<12> near_plane, Fixed32<12> far_plane, uint16 resolution);

        void add_node(OrderingTableNode* node, td::Fixed32<12> z);

    private:

        [[nodiscard]] static uint16 compute_total_resolution(const List<OrderingTableLayerPart>& parts);

        const Fixed32<12> near_plane;
        const Fixed32<12> far_plane;

        List<OrderingTableLayerPart> parts;
        List<OrderingTableNode> ordering_table;

        friend class OrderingTable;

    };

}