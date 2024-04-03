#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/list.dec.hpp>

#include <tundra/rendering/ordering-table-node.hpp>
#include <tundra/rendering/ordering-table-layer.hpp>

namespace td {

    class OrderingTable {
    public:
    
        OrderingTable(const List<OrderingTableLayerSettings>& layers);

        OrderingTable(const OrderingTable&) = delete;

        void clear();

        OrderingTableLayer& get_layer(uint32 layer_index);

        [[nodiscard]] uint32 get_num_layers() const;

        // Returns the first node in the linked list, which is the first node
        // to draw (i.e. the node that is furthest to the back in the last layer).
        [[nodiscard]] const OrderingTableNode* get_first_node_to_draw() const;

    private:

        List<OrderingTableLayer> layers;
        List<OrderingTableNode> root_ordering_table;

    };

};