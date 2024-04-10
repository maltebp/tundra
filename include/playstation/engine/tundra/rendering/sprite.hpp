#pragma once

#include "tundra/core/fixed.hpp"
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/vec/vec3.hpp>

#include <tundra/engine/entity-system/component.dec.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

    // TODO: Implement proper transform system for screen space stuff (e.g. sprites)

    class Sprite : public Component<Sprite> {
    public:

        Sprite(uint32 layer_index) : layer_index(layer_index) { }

        const uint32 layer_index;

        const TextureAsset* texture = nullptr;

        bool enabled = true;

        // Position of center of sprite 0, 0 is top left corner and 320,240 is bottom right
        Vec2<Fixed32<12>> position { 0 };
        Vec2<Fixed32<12>> size { 16 };
        
        // Counter-clockwise, where 1 is 360 degrees
        Fixed16<12> rotation = 0;

        Vec3<uint8> color { 255, 255, 255 };

    };

}