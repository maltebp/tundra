#pragma once

#include <psxgte.h>
#include <psxgpu.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/list.hpp>
#include <tundra/engine/rendering/ordering-table-layer.hpp>
#include <tundra/engine/rendering/ordering-table-node.hpp>
#include <tundra/engine/rendering/ordering-table.hpp>

#include "array.hpp"
#include "initialize-prim.hpp"



class RenderBuffer {
public:

    RenderBuffer(
        td::uint16 ordering_table_size, // Full size, including foreground and background 
        size_t primitives_buffer_size, 
        int display_x, int display_y, 
        int draw_x, int draw_y, 
        int framebuffer_width, int framebuffer_height, 
        CVECTOR clear_color);

    ~RenderBuffer();

    // Allocate memory for the prim, initializes it and adds it to the ordering table
    template<typename TPrim>
    TPrim* create_and_add_prim(td::uint16 resolution_index);

    template<typename TPrim>
    TPrim* create_and_add_background_prim();

    template<typename TPrim>
    TPrim* create_and_add_foreground_prim();

    td::OrderingTableLayer& get_middle_layer();

    void draw();

    void clear();    

    void activate();

    // Because of the annoying FntSort I have to expose these
    uint8_t* next_primitive_ptr;
    Array<uint8_t> primitives_buffer;

private: 

    DISPENV display_environment;
	DRAWENV drawing_environment;

    td::OrderingTable ordering_table;
};


template<typename TPrim>
TPrim* RenderBuffer::create_and_add_prim(td::uint16 resolution_index) {
    TD_ASSERT(
        next_primitive_ptr + sizeof(TPrim) < primitives_buffer.get_data_end(),
        "Render buffer's primitive buffer is full");
    td::OrderingTableLayer& layer = get_middle_layer();

    TD_ASSERT(resolution_index < layer.get_resolution(), "resolution index must be less than %d (was %d)", layer.get_resolution(), resolution_index);

    TPrim* prim = (TPrim*)next_primitive_ptr;
    next_primitive_ptr += sizeof(TPrim);
    initialize_prim(prim);

    td::OrderingTableNode* prim_ordering_table_node = (td::OrderingTableNode*)prim;
    layer.add_node(prim_ordering_table_node, resolution_index);

    return prim;
}

template<typename TPrim>
TPrim* RenderBuffer::create_and_add_background_prim() {
    TD_ASSERT(
        next_primitive_ptr + sizeof(TPrim) < primitives_buffer.get_data_end(),
        "Render buffer's primitive buffer is full");

    TPrim* prim = (TPrim*)next_primitive_ptr;
    next_primitive_ptr += sizeof(TPrim);
    initialize_prim(prim);

    td::OrderingTableNode* prim_ordering_table_node = (td::OrderingTableNode*)prim;
    ordering_table.get_layer(0).add_to_front(prim_ordering_table_node);
    
    return prim;
}

template<typename TPrim>
TPrim* RenderBuffer::create_and_add_foreground_prim() {
    TD_ASSERT(
        next_primitive_ptr + sizeof(TPrim) < primitives_buffer.get_data_end(),
        "Render buffer's primitive buffer is full");

    TPrim* prim = (TPrim*)next_primitive_ptr;
    next_primitive_ptr += sizeof(TPrim);
    initialize_prim(prim);

    td::OrderingTableNode* prim_ordering_table_node = (td::OrderingTableNode*)prim;
    ordering_table.get_layer(2).add_to_front(prim_ordering_table_node);
    
    return prim;
}