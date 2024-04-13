#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/string.hpp>

#include <tundra/engine/entity-system/component.dec.hpp>

namespace td {

    class Text : public Component<Text> {
    public:

        Text(uint32 layer_index) : layer_index(layer_index) { }

        const uint32 layer_index = 0;

        bool is_enabled = true;

        td::String text = "";

        // Top left corner of Text in screen space (0, 0 is top left and positive y-axis is down)
        Vec2<Fixed32<12>> position { 0 };

    };

}