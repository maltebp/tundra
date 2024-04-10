#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vec/vec2.dec.hpp>

namespace td {

    struct TextureAssetLoadInfo {
    public:

        uint16 texture_page_id;

        Vec2<uint8> texture_page_offset;

        uint16 clut_id;

    };

}