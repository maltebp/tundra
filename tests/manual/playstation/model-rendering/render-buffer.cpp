#include "render-buffer.hpp"

RenderBuffer::RenderBuffer(
    size_t ordering_table_size, 
    size_t primitives_buffer_size, 
    int display_x, int display_y, 
    int draw_x, int draw_y, 
    int framebuffer_width, int framebuffer_height, 
    CVECTOR clear_color
)
    :   ordering_table(ordering_table_size),
        primitives_buffer(primitives_buffer_size)
{
    // TODO: Does it make sense to differ in draw and display coordinates?

    /* Set display and draw environment areas */
	/* (display and draw areas must be separate, otherwise hello flicker) */
	SetDefDispEnv( &display_environment, display_x, display_y, framebuffer_width, framebuffer_height );
	SetDefDrawEnv( &drawing_environment, draw_x, draw_y, framebuffer_width, framebuffer_height);
	
	/* Enable draw area clear and dither processing */
	setRGB0( &drawing_environment, 63, 0, 127 );
	drawing_environment.isbg = 1;
	drawing_environment.dtd = 1;
    drawing_environment.r0 = clear_color.r;
    drawing_environment.g0 = clear_color.g;
    drawing_environment.b0 = clear_color.b;
}

void RenderBuffer::clear() {
    next_primitive_ptr = primitives_buffer.get_data();
    ClearOTagR(ordering_table.get_data(), ordering_table.get_size());
}

void RenderBuffer::draw() {
    DrawOTag(&ordering_table.get_last());
}

void RenderBuffer::activate() {
    PutDrawEnv( &drawing_environment );
    PutDispEnv(&display_environment);
}