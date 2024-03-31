#pragma once

#include <stddef.h>
#include <psxgte.h>

#include "render-buffer.hpp"
#include "look-at-camera.hpp"

class Box;
class Model;
class Image;


class Renderer {
public:

    Renderer() = default;

    Renderer(const Renderer&) = delete;

    void initialize(
        td::uint16 ordering_table_size,
        size_t primitives_buffer_size,
        int resolution_width, 
        int resolution_height,
        CVECTOR clear_color
    );

    void draw_model(const Model& model); 

    void flush();

    // normalized_direction is in 4.12 format
    void set_light_direction(uint8_t light_index, SVECTOR normalized_direction);

    // color is in 4.12 format, and must be between 0 and 1
    void set_light_color(uint8_t light_index, VECTOR color);

    LookAtCamera& get_camera() { return camera; }

private:

    bool screen_triangle_is_in_screen(
        const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2
    ) const;

    bool screen_quad_is_in_screen(
        const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2, const DVECTOR* v3
    ) const;

private:

    LookAtCamera camera;

    bool initialized = true;

    uint16_t resolution_width;
    uint16_t resolution_height;

    RenderBuffer* render_buffer_1;
    RenderBuffer* render_buffer_2;
    RenderBuffer* active_render_buffer;

    MATRIX light_directions = {};
    MATRIX light_colors = {};
    bool light_colors_changed = false;

    int32_t num_rendered_triangles = 0;

};