#include <tundra/rendering/camera.hpp>

#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/assert.hpp>

#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/ordering-table-layer.hpp>
#include <tundra/rendering/ordering-table.hpp>

namespace td {

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
            ordering_tables{get_ordering_table_settings(layers_to_render), get_ordering_table_settings(layers_to_render)}
    { 
        TD_ASSERT(layers_to_render.get_size() > 0, "Must render at least one layer");
    }

    OrderingTableLayer& Camera::get_ordering_table_layer(DoubleBufferId ordering_table_id, uint32 layer_id) {
        TD_ASSERT(layers_to_render.contains(layer_id), "Camera does not render layer %d", layer_id);
        return ordering_tables[(uint8)ordering_table_id].get_layer(layer_id);
    }

    void Camera::look_at(const Vec3<Fixed32<12>>&) {
        
        // TODO: Implement
    }
    
}