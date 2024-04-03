#pragma once

#include <tundra/core/vec/vec3.hpp>

#include <tundra/engine/transform-matrix.hpp>

#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/primitive-buffer.hpp>
#include <tundra/rendering/ordering-table.hpp>
#include <tundra/rendering/camera.fwd.hpp>

namespace td {

    class Model;

    class RenderSystem {
    public:

        RenderSystem(
            uint32 primitive_buffer_size,
            Vec3<uint8> clear_color
        );

        void render();

    private:

        void render_camera(Camera* camera);    

        void render_model(const TransformMatrix& camera_matrix, const Model* model, OrderingTableLayer& ordering_table_layer);

        DoubleBufferId active_buffer = DoubleBufferId::First;

        PrimitiveBuffer primitive_buffers[2];
        Vec3<uint16> clear_color;

        Mat3x3<Fixed16<12>> light_directions;


    };

}