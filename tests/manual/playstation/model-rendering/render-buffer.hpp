#pragma once

#include <psxgte.h>
#include <psxgpu.h>

#include <tundra/core/assert.hpp>

#include "array.hpp"
#include "initialize-prim.hpp"


class RenderBuffer {
public:

    RenderBuffer(
        size_t ordering_table_size, // Full size, including foreground and background 
        size_t primitives_buffer_size, 
        int display_x, int display_y, 
        int draw_x, int draw_y, 
        int framebuffer_width, int framebuffer_height, 
        CVECTOR clear_color);

    ~RenderBuffer();

    // Allocate memory for the prim, initializes it and adds it to the ordering table
    template<typename TPrim>
    TPrim* create_and_add_prim(int16_t z);

    template<typename TPrim>
    TPrim* create_and_add_background_prim();

    template<typename TPrim>
    TPrim* create_and_add_foreground_prim();


    void draw();

    void clear();    

    void activate();

    // Size minus foreground and background
    size_t ordering_table_size() const { return ordering_table.get_size() - 2; }

    uint32_t& get_foreground_ordering_table_entry() { return ordering_table.get_first(); }

    // Because of the annoying FntSort I have to expose these
    uint8_t* next_primitive_ptr;
    Array<uint8_t> primitives_buffer;

private: 

    DISPENV display_environment;
	DRAWENV drawing_environment;

    Array<uint32_t> ordering_table;
};


template<typename TPrim>
TPrim* RenderBuffer::create_and_add_prim(int16_t z) {
    TD_ASSERT(
        next_primitive_ptr + sizeof(TPrim) < primitives_buffer.get_data_end(),
        "Render buffer's primitive buffer is full");

    TD_ASSERT(z >= 0, "Z value must be larger than or equal to 0 (was %d)", z);

    TD_ASSERT(z < ordering_table_size(), "z value must be less than %d (was %d)", ordering_table_size(), z);

    // 0 is reserved for foreground prims
    int adjusted_z = z + 1;

    TPrim* prim = (TPrim*)next_primitive_ptr;
    next_primitive_ptr += sizeof(TPrim);
    initialize_prim(prim);
    
    uint32_t* ordering_table_entry = &ordering_table[z];
    addPrim(ordering_table_entry, prim);

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
    
    uint32_t* ordering_table_entry = &ordering_table.get_last();
    addPrim(ordering_table_entry, prim);
    
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
    
    uint32_t* ordering_table_entry = &ordering_table.get_first();
    addPrim(ordering_table_entry, prim);
    
    return prim;
}