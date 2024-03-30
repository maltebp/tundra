#pragma once

#include "tundra/engine/dynamic-transform.hpp"
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.dec.hpp>

namespace td {

    class Camera : public Component<Camera> {
    public:

        Camera(const ComponentRef<DynamicTransform>& transform) 
            :   transform(transform) 
        { }

        // Depth resolution
        // 
        // Layers to render

    private:

        const ComponentRef<DynamicTransform> transform;

    };

}