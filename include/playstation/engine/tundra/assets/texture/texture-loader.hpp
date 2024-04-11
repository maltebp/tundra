#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/grid-allocator.fwd.hpp>

#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

    namespace texture_loader {
    
        extern const ::td::TextureAsset* load_texture(VramAllocator& vram_allocator, const byte* raw_data);
    
    }

}