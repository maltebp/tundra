#include "tundra/core/log.hpp"
#include <tundra/assets/texture/texture-loader.hpp>

#include <psxgpu.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/limits.hpp>
#include <tundra/core/grid-allocator.hpp>

#include <tundra/assets/texture/texture-asset.hpp>
#include <tundra/assets/texture/texture-asset-load-info.hpp>

namespace td {

    const uint32 TIM_TYPE_TRUE_COLOR = 0x08;
    const uint32 TIM_TYPE_PALETTE_4_BIT = 0x09;
    const uint32 TIM_TYPE_PALETTE_8_BIT = 0x02;

    namespace internal {

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

    extern const TextureAsset* texture_loader::load_texture(GridAllocator& vram_allocator, const byte* raw_data) {
        TD_ASSERT(raw_data != nullptr, "Texture raw data must not be nullptr");

        TextureAsset* asset = new TextureAsset();
        asset->load_info = new TextureAssetLoadInfo();

        TIM_IMAGE tim;

        GetTimInfo((const uint32*)raw_data, &tim);

        bool has_clut = tim.mode == TIM_TYPE_PALETTE_4_BIT || tim.mode == TIM_TYPE_PALETTE_8_BIT;
        if( has_clut ) {
            GridAllocator::Result clut_vram_allocation_result = vram_allocator.allocate(
                (uint16)tim.crect->w, 
                (uint16)tim.crect->h
            );

            TD_ASSERT(clut_vram_allocation_result.success, "Failed to allocate texture's CLUT in VRAM of size %d x %d", tim.crect->w, tim.crect->h);

            RECT clut_vram_rect;
            clut_vram_rect.x = clut_vram_allocation_result.position.x;
            clut_vram_rect.y = clut_vram_allocation_result.position.y;
            clut_vram_rect.w = tim.crect->w;
            clut_vram_rect.h = tim.crect->h;

            // Load CLUT to VRAM
            LoadImage(&clut_vram_rect, tim.caddr);

            // TODO: x must be a multiple of 16 units
            asset->load_info->clut_id = (uint16)getClut(clut_vram_rect.x, clut_vram_rect.y); 
        }

        GridAllocator::Result pixels_vram_allocation_result = vram_allocator.allocate(
            (uint16)tim.prect->w, 
            (uint16)tim.prect->h
        );

        TD_ASSERT(pixels_vram_allocation_result.success, "Failed to allocate texture's pixels in VRAM of size %d x %d", tim.prect->w, tim.prect->h);

        RECT pixels_vram_rect;
        pixels_vram_rect.x = pixels_vram_allocation_result.position.x;
        pixels_vram_rect.y = pixels_vram_allocation_result.position.y;
        pixels_vram_rect.w = tim.prect->w;
        pixels_vram_rect.h = tim.prect->h;
        
        // Load pixels to VRAM
        LoadImage(&pixels_vram_rect, tim.paddr);

        asset->load_info->texture_page_id = (uint16)internal::get_texture_page(tim.mode, 1, (uint16)pixels_vram_rect.x, (uint16)pixels_vram_rect.y);

        asset->pixels_width = (uint16)tim.prect->w;
        asset->pixels_height = (uint16)tim.prect->h;

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