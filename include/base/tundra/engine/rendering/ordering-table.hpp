#pragma once

#include <tundra/engine/rendering/ordering-table-node.hpp>
#include <tundra/engine/rendering/ordering-table-layer.hpp>

namespace td {

    class OrderingTable {
    public:
    
        OrderingTable(const List<OrderingTableLayer>& layers);

        void clear();

        OrderingTableLayer& get_layer(uint32 layer_index);

    private:

        List<OrderingTableLayer> layers;
        List<OrderingTableNode> root_ordering_table;

    };

};