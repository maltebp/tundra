#include "tundra/core/fixed.hpp"
#include "tundra/gte/operations.hpp"
#include <tundra/rendering/camera.hpp>

#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/mat/mat3x3.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/math.hpp>

#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/ordering-table-layer.hpp>
#include <tundra/rendering/ordering-table.hpp>

namespace td {

    constexpr static uint16 field_of_view_to_h_register_value(td::Fixed32<12> field_of_view) {
        constexpr td::Fixed32<12> WIDTH = 320;
        return (uint16)((WIDTH * td::to_fixed(0.5)) / td::tan_degrees(field_of_view / 2)).get_raw_integer();
    }        

    List<uint32> get_layer_ids(const List<CameraLayerSettings>& layer_settings) {
        List<uint32> layer_ids;
        for( uint32 i = 0; i < layer_settings.get_size(); i++ ) {
            layer_ids.add(layer_settings[i].layer_index);
        }
        return layer_ids;
    }

    List<OrderingTableLayerSettings> get_ordering_table_settings(const List<CameraLayerSettings>& layer_settings) {
        List<OrderingTableLayerSettings> ordering_table_layer_settings;
        for( uint32 i = 0; i < layer_settings.get_size(); i++ ) {
            ordering_table_layer_settings.add({ layer_settings[i].depth_resolution, layer_settings[i].max_distance });
        }
        return ordering_table_layer_settings;
    }

    Camera::Camera(
        const ComponentRef<DynamicTransform>& transform,
        const List<CameraLayerSettings>& layers_to_render
    )
        :   transform(transform),
            layers_to_render(get_layer_ids(layers_to_render)),
            ordering_tables{get_ordering_table_settings(layers_to_render), get_ordering_table_settings(layers_to_render)},
            near_plane_distance(field_of_view_to_h_register_value(field_of_view_degrees))
    { 
        TD_ASSERT(layers_to_render.get_size() > 0, "Must render at least one layer");
    }

    OrderingTableLayer& Camera::get_ordering_table_layer(DoubleBufferId ordering_table_id, uint32 layer_id) {
        TD_ASSERT(layers_to_render.contains(layer_id), "Camera does not render layer %d", layer_id);
        uint32 ordering_table_layer_index = layers_to_render.index_of(layer_id);
        return ordering_tables[(uint8)ordering_table_id].get_layer(ordering_table_layer_index);
    }

    void Camera::set_field_of_view(td::Fixed32<12> field_of_view_degrees) {
        TD_ASSERT(field_of_view_degrees > 0, "Field of view must be larger than 0 (was %s)", field_of_view_degrees);
        TD_ASSERT(field_of_view_degrees < 180, "Field of view must be less than 180 (was %s)", field_of_view_degrees);
        this->field_of_view_degrees = field_of_view_degrees;
        near_plane_distance = field_of_view_to_h_register_value(field_of_view_degrees);
    }

    void Camera::look_at(const Vec3<Fixed32<12>>& target) {
        // TODO: Should be moved to GTE
        TD_ASSERT(transform != nullptr, "Camera transform has been destroyed");
        TD_ASSERT(target != transform->get_translation(), "Cannot look at target when it is the same as position");

        // TODO: Is this normalization necessary? (I don't think so)
        Vec3<Fixed16<12>> direction_to_target = gte::normalize(target - transform->get_translation());
        TD_ASSERT(direction_to_target.x != 0 || direction_to_target.z != 0, "Camera cannot look at target directly above it");
        
        // Find y_angle by finding the angle of the direction when looking down the y-axis (i.e.
        // viewing from below, so z-axis points to the right and x-axis points up)
        // We do this because atan2 assumes counter-clockwise rotation in its 2D grid, but looking
        // from above (down the negative y-axis) the rotation around y-axis is clockwise
        Fixed16<12> y_angle = atan2(direction_to_target.x, direction_to_target.z);

        Fixed16<12> c = cos(-y_angle);
        Fixed16<12> s = sin(-y_angle);
        Fixed16<12> local_z_after_y_rotate = c * direction_to_target.z - s * direction_to_target.x;

        Fixed16<12> x_angle = 
            direction_to_target.y == 0 ? 0 : -atan2(direction_to_target.y, local_z_after_y_rotate);
        transform->set_rotation({x_angle, y_angle, transform->get_rotation().z});        
    }

    
    
}