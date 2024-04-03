#include "render-buffer.hpp"

#include <tundra/rendering/ordering-table-layer.hpp>

td::List<td::OrderingTableLayer> create_layers(td::uint16 mid_layer_resolution) {

    td::List<td::OrderingTableLayer> layers;
    
    layers.add(td::OrderingTableLayer{1}); // Foreground
    layers.add(td::OrderingTableLayer{mid_layer_resolution});
    layers.add(td::OrderingTableLayer{1}); // Background

    return layers;
}

RenderBuffer::RenderBuffer(
    td::uint16 ordering_table_size, 
    size_t primitives_buffer_size, 
    int display_x, int display_y, 
    int draw_x, int draw_y, 
    int framebuffer_width, int framebuffer_height, 
    CVECTOR clear_color
)
    :   primitives_buffer(primitives_buffer_size),
        ordering_table(create_layers(ordering_table_size))
{
    // TODO: Does it make sense to differ in draw and display coordinates?

    /* Set display and draw environment areas */
	/* (display and draw areas must be separate, otherwise hello flicker) */
	SetDefDispEnv( &display_environment, display_x, display_y, framebuffer_width, framebuffer_height );
	SetDefDrawEnv( &drawing_environment, draw_x, draw_y, framebuffer_width, framebuffer_height);
	
	/* Enable draw area clear and dither processing */
	setRGB0( &drawing_environment, 63, 0, 127 );
	drawing_environment.isbg = 1;
	drawing_environment.dtd = 1; // TODO: Disable dithering
    drawing_environment.r0 = clear_color.r;
    drawing_environment.g0 = clear_color.g;
    drawing_environment.b0 = clear_color.b;
}

void RenderBuffer::clear() {
    next_primitive_ptr = primitives_buffer.get_data();
    ordering_table.clear();
}

void RenderBuffer::draw() {
    DrawOTag((const td::uint32*)ordering_table.get_first_node_to_draw());
}

void RenderBuffer::activate() {
    PutDrawEnv( &drawing_environment );
    PutDispEnv(&display_environment);
}

td::OrderingTableLayer& RenderBuffer::get_middle_layer() {
    return ordering_table.get_layer(1);
}