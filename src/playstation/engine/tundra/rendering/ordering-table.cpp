#include <tundra/rendering/ordering-table.hpp>

#include <psxgpu.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/list.hpp>
#include <tundra/rendering/ordering-table-layer.hpp>


namespace td {

    List<OrderingTableLayer> create_layers(const List<OrderingTableLayerSettings>& settings) {
        List<OrderingTableLayer> layers;
        for( uint32 i = 0; i < settings.get_size(); i++ ) {
            layers.add({settings[i].resolution, settings[i].far_plane});
        }
        return layers;
    }

    OrderingTable::OrderingTable(const List<OrderingTableLayerSettings>& layers) 
        :   layers(td::move(create_layers(layers))),
            root_ordering_table(layers.get_size())
    { 
        TD_ASSERT(layers.get_size() > 0, "There must be at least 1 layer");
        clear();
    }

    void OrderingTable::clear() {
        ClearOTagR((uint32_t*)root_ordering_table.get_data(), root_ordering_table.get_size());
        
        for( uint32 i = 0; i < layers.get_size(); i++ ) {
            OrderingTableNode& layer_front = layers[i].ordering_table[0];
            OrderingTableNode& layer_back = layers[i].ordering_table.get_last();

            ClearOTagR((uint32_t*)&layer_front, layers[i].ordering_table.get_size());

            // Insert layer's ordering table as a node into slot i of the root ordering table
            layer_front.next_node_ptr = root_ordering_table[i].next_node_ptr;
            root_ordering_table[i].next_node_ptr = layer_back.next_node_ptr;
            // Using this instead of psn00bsdk's addPrims, because it was throwing warnings

            layers[i].front_node = &layer_front; 
            layers[i].num_added_nodes = 0;
        }
    }

    OrderingTableLayer& OrderingTable::get_layer(uint32 layer_index) {
        TD_ASSERT(layer_index < layers.get_size(), "Layer index out of bounds (was %d when there are %d layers)", layer_index, layers.get_size());
        return layers[layer_index];
    }

    [[nodiscard]] uint32 OrderingTable::get_num_layers() const {
        return layers.get_size();
    }

    [[nodiscard]] const OrderingTableNode* OrderingTable::get_first_node_to_draw() const {
        return &root_ordering_table.get_last();
    }

}