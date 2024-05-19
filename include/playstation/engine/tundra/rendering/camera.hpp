#pragma once

#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.dec.hpp>
#include <tundra/rendering/double-buffer-id.hpp>
#include <tundra/rendering/ordering-table-layer.hpp>
#include <tundra/rendering/ordering-table.hpp>

namespace td {

    class RenderSystem;

    struct CameraLayerSettings {
            
        CameraLayerSettings(
            td::uint16 layer_index, 
            uint16 depth_resolution, 
            UFixed16<12> max_distance = td::limits::numeric_limits<UFixed16<12>>::max
        ) 
            :   layer_index(layer_index),
                depth_resolution(depth_resolution),
                max_distance(max_distance)
        { }

        td::uint16 layer_index;
        uint16 depth_resolution;
        UFixed16<12> max_distance = td::limits::numeric_limits<UFixed16<12>>::max;
    };

    class Camera : public Component<Camera> {
    public:

        Camera(
            const ComponentRef<DynamicTransform>& transform,
            const List<CameraLayerSettings>& layers_settings
        );

        OrderingTableLayer& get_ordering_table_layer(DoubleBufferId ordering_table_id, uint32 layer_id);

        void set_field_of_view(td::Fixed32<12> field_of_view);

        td::Fixed32<12> get_field_of_view() { return field_of_view_degrees; }

        void look_at(const Vec3<Fixed32<12>>& target);

        const ComponentRef<DynamicTransform> transform;

        const List<uint32> layers_to_render;

    private:

        OrderingTable ordering_tables[2];

        td::Fixed32<12> field_of_view_degrees = 60;

        // Cached calculation based on fov
        uint16 near_plane_distance; 

        friend class RenderSystem;
        
    };

}