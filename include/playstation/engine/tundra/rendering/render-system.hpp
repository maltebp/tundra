#pragma once

#include <psxgpu.h>

#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/grid-allocator.fwd.hpp>

#include <tundra/engine/transform-matrix.hpp>

#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/primitive-buffer.hpp>
#include <tundra/rendering/ordering-table.hpp>
#include <tundra/rendering/camera.fwd.hpp>

namespace td {

    class Model;
    class Sprite;

    class RenderSystem {
    public:

        RenderSystem(
            GridAllocator& vram_allocator,
            uint32 primitive_buffer_size,
            Vec3<uint8> clear_color
        );

        void render();

        // The color is both the intensity of the light and the hue/saturation
        void set_ambient_light(Vec3<uint8> color);

        // Light_index must be between 0 and 2, and direction must be normalized
        void set_light_direction(uint8 light_index, Vec3<Fixed16<12>> direction);
        
        // Light_index must be between 0 and 2, and the color describe both the
        // intensity of the light and the hue/saturation
        void set_light_color(uint8 light_index, Vec3<uint8> color);

    private:

        void render_camera(Camera* camera);    

        void render_sprite(const Sprite* model, OrderingTableLayer& ordering_table_layer);

        void render_model(const TransformMatrix& camera_matrix, const Model* model, OrderingTableLayer& ordering_table_layer);
        
        const Vec2<uint16> frame_buffer_positions[2];
        
        DoubleBufferId active_buffer = DoubleBufferId::First;

        PrimitiveBuffer primitive_buffers[2];
        Vec3<uint8> clear_color;

        Vec3<uint8> ambient_color;
        
        // Each row is the direction of a light
        alignas(4) Mat3x3<Fixed16<12>> light_directions = {};

        // Each column is the color of a light
        alignas(4) Mat3x3<Fixed16<12>> light_colors;

    };

}