#include <tundra/rendering/camera.hpp>

#include <tundra/core/assert.hpp>

namespace td {

    Camera::Camera(
        const ComponentRef<DynamicTransform>& transform,
        const List<uint32>& layers_to_render,
        const List<OrderingTableLayer>& layers_settings
    )
        :   transform(transform),
            layers_to_render(layers_to_render),
            ordering_table_1(layers_settings),
            ordering_table_2(layers_settings)
    { 
        TD_ASSERT(layers_to_render.get_size() > 0, "Must render at least one layer");

        TD_ASSERT(
            layers_to_render.get_size() == layers_settings.get_size(), 
            "Mismatch between number of layers to render (%d) and number of layer settings (%d)",
            layers_to_render.get_size(), layers_settings.get_size()
        );
    }
    
}