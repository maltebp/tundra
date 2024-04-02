#pragma once

#include <tundra/rendering/ordering-table-layer.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.dec.hpp>
#include <tundra/rendering/ordering-table.hpp>

namespace td {

    class Camera : public Component<Camera> {
    public:

        Camera(
            const ComponentRef<DynamicTransform>& transform,
            const List<uint32>& layers_to_render
        ) 
            :   Camera(transform, layers_to_render, List<OrderingTableLayer>{})  
        { }

        Camera(
            const ComponentRef<DynamicTransform>& transform,
            const List<uint32>& layers_to_render,
            const List<OrderingTableLayer>& layers_settings
        );

    private:

        const ComponentRef<DynamicTransform> transform;

        const List<uint32> layers_to_render;

        OrderingTable ordering_table_1;
        OrderingTable ordering_table_2;
        
    };

}