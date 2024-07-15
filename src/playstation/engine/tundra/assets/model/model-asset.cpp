#include "tundra/core/fixed.hpp"
#include <tundra/assets/model/model-asset.hpp>

#include <tundra/core/list.hpp>

#include <tundra/assets/texture/texture-asset-load-info.hpp>

namespace td {
    void ModelAsset::map_to_texture(const TextureAsset* texture) {
        TD_ASSERT(texture != nullptr, "Texture must not be nullptr");
        TD_ASSERT(this->num_textures > 0, "ModelAsset does not need any textures");
        TD_ASSERT(this->num_uvs > 0, "ModelAsset has no UVs");
        TD_ASSERT(this->uvs != nullptr, "ModelAsset's UV list is nullptr");
        TD_ASSERT(this->texture == nullptr, "ModelAsset already has is texture set");

        this->texture = texture;
        
        for( uint16 i = 0; i < num_uvs; i++ ) {
            td::Vec2<Fixed16<12>> uv {
                Fixed16<12>::from_raw_fixed_value(uvs[i].x),
                Fixed16<12>::from_raw_fixed_value(uvs[i].y),
            };

            // Flipping y (framebuffer has 0,0 in top-left, obj has 0,0 in bottom left)
            uv.y = Fixed16<12>{1} - uv.y;

            Vec2<uint8> mapped_uv {
                (uint8)td::round(Fixed32<12>{uv.x} * (texture->pixels_width - 1)).get_raw_integer(),
                (uint8)td::round(Fixed32<12>{uv.y} * (texture->pixels_height - 1)).get_raw_integer(),
            };
            
            // Making them relative to texture page 
            mapped_uv += {
                texture->load_info->texture_page_offset.x,
                texture->load_info->texture_page_offset.y
            };

            mapped_uvs.add(mapped_uv);
        }
    }
}