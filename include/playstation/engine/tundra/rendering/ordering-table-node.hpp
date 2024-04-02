#pragma once

#include <tundra/core/types.hpp>

namespace td {

    struct OrderingTableNode {
    public:
        uint32 next_node_ptr : 24;
        uint32 data_size : 8;

        void set_next_node(OrderingTableNode* next_node) {
            next_node_ptr = (uint32)(next_node) & 0x00FFFFFF;
        }
    };

    static_assert(sizeof(OrderingTableNode) == 4, "");
}