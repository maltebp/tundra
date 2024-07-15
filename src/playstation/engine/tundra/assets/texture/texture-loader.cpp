#include "tundra/core/log.hpp"
#include "tundra/rendering/vram-allocator.hpp"
#include <tundra/assets/texture/texture-loader.hpp>

#include <psxgpu.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/limits.hpp>
#include <tundra/core/grid-allocator.hpp>
#include <tundra/core/vec/vec2.hpp>

#include <tundra/assets/texture/texture-asset.hpp>
#include <tundra/assets/texture/texture-asset-load-info.hpp>

namespace td {

    const uint32 TIM_TYPE_TRUE_COLOR = 0x02;
    const uint32 TIM_TYPE_PALETTE_4_BIT = 0x08;
    const uint32 TIM_TYPE_PALETTE_8_BIT = 0x09;

    namespace internal {

        constexpr TextureMode tim_mode_to_texture_mode(uint32 tim_mode) {

            switch(tim_mode) {
                case TIM_TYPE_TRUE_COLOR: return TextureMode::TrueColor;
                case TIM_TYPE_PALETTE_4_BIT: return TextureMode::Palette4Bit;
                case TIM_TYPE_PALETTE_8_BIT: return TextureMode::Palette8Bit;
            }

            TD_ASSERT(false, "Unknown tim_mode %d", tim_mode);

            return TextureMode::TrueColor;
        }

        constexpr uint16 get_texture_page(uint32 tp, uint32 abr, uint32 x, uint32 y) {
            // Own implementation of getTPage, because it had signedness warnings

            uint32 raw = 
                (((x) & 0x3c0) >> 6) |
	            (((y) & 0x100) >> 4) |
	            (((y) & 0x200) << 2) |
	            (((abr) & 3) << 5) |
	            (((tp)  & 3) << 7);

            TD_ASSERT(
                raw <= td::limits::numeric_limits<uint16>::max,
                "Texture Page ID was higher than uint16 (was %d) - did I misunderstand something here?", raw);

            return (uint16)raw;
        }

    }

    extern const TextureAsset* texture_loader::load_texture(VramAllocator& vram_allocator, const byte* raw_data) {
        TD_ASSERT(raw_data != nullptr, "Texture raw data must not be nullptr");

        TextureAsset* asset = new TextureAsset();
        asset->load_info = new TextureAssetLoadInfo();

        TIM_IMAGE tim;
        GetTimInfo((const uint32*)raw_data, &tim);

        asset->mode = internal::tim_mode_to_texture_mode(tim.mode);

        bool has_clut = asset->mode == TextureMode::Palette4Bit || asset->mode == TextureMode::Palette8Bit;
        if( has_clut ) {
            Vec2<uint16> clut_position = asset->mode == TextureMode::Palette4Bit ? vram_allocator.allocate_clut_4() : vram_allocator.allocate_clut_8();

            RECT clut_vram_rect;
            clut_vram_rect.x = (int16)clut_position.x;
            clut_vram_rect.y = (int16)clut_position.y;
            clut_vram_rect.w = tim.crect->w;
            clut_vram_rect.h = tim.crect->h;

            // Load CLUT to VRAM
            LoadImage(&clut_vram_rect, tim.caddr);

            asset->load_info->clut_id = (uint16)getClut(clut_vram_rect.x, clut_vram_rect.y); 
        }

        TextureAllocationResult texture_page_allocation = vram_allocator.allocate_texture(asset->mode, (uint8)tim.prect->w, (uint8)tim.prect->h);

        RECT pixels_vram_rect;
        pixels_vram_rect.x = (int16)texture_page_allocation.position_in_vram.x;
        pixels_vram_rect.y = (int16)texture_page_allocation.position_in_vram.y;
        pixels_vram_rect.w = tim.prect->w;
        pixels_vram_rect.h = tim.prect->h;

        asset->load_info->texture_page_offset = {
            (uint8)texture_page_allocation.position_in_texture_page.x ,
            (uint8)texture_page_allocation.position_in_texture_page.y
        };
        
        // Load pixels to VRAM
        LoadImage(&pixels_vram_rect, tim.paddr);

        asset->load_info->texture_page_id = internal::get_texture_page(tim.mode, 1, (uint16)pixels_vram_rect.x, (uint16)pixels_vram_rect.y);

        asset->pixels_width = (uint16)tim.prect->w;
        asset->pixels_height = (uint16)tim.prect->h;

        if( asset->mode == TextureMode::Palette4Bit ) {
            asset->pixels_width <<= 2;
            asset->load_info->texture_page_offset.x <<= 2;
        }
        if( asset->mode == TextureMode::Palette8Bit ) {
            asset->pixels_width <<= 1;
            asset->load_info->texture_page_offset.x <<= 1;
        }

        switch(tim.mode) {
            case TIM_TYPE_TRUE_COLOR:
                asset->mode = TextureMode::TrueColor;
                break;
            case TIM_TYPE_PALETTE_4_BIT:
                asset->mode = TextureMode::Palette4Bit;
                break;
            case TIM_TYPE_PALETTE_8_BIT:
                asset->mode = TextureMode::Palette4Bit;
                break;
            default:
                TD_ASSERT(false, "TIM mode %d did has to TextureMode equivalent", tim.mode);
        }
	    
        return asset;
    }

}