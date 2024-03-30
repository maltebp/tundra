#include <tundra/engine/rendering/ordering-table.hpp>

#include <psxgpu.h>

#include <tundra/core/assert.hpp>
#include <tundra/engine/rendering/ordering-table-layer.hpp>


namespace td {

    OrderingTable::OrderingTable(const List<OrderingTableLayer>& layers) 
        :   layers(layers),
            root_ordering_table(layers.get_size())
    { 
        TD_ASSERT(layers.get_size() > 0, "There must be at least 1 layer");
        clear();
    }

    void OrderingTable::clear() {
        ClearOTagR((uint32_t*)root_ordering_table.get_data(), root_ordering_table.get_size());
        
        for( uint32 i = 0; i < layers.get_size(); i++ ) {
            OrderingTableNode* layer_start = &layers[i].ordering_table[0];
            OrderingTableNode* layer_end = &layers[i].ordering_table.get_last();

            ClearOTagR((uint32_t*)layer_start, layers[i].ordering_table.get_size());
            addPrims(&root_ordering_table[i], layer_end, layer_start);
        }
    }

    OrderingTableLayer& OrderingTable::get_layer(uint32 layer_index) {
        TD_ASSERT(layer_index < layers.get_size(), "Layer index out of bounds (was %d when there are %d layers)", layer_index, layers.get_size());
        return layers[layer_index];
    }

}