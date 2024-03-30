#pragma once

#include <tundra/core/types.hpp>

namespace td {

    struct OrderingTableNode {
    public:
        uint32 next_node_ptr : 24;
        uint32 data_size : 8;
    };

    static_assert(sizeof(OrderingTableNode) == 4, "");

}