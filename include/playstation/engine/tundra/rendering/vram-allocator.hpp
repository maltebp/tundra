#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/list.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/grid-allocator.hpp>
#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

     struct TextureAllocationResult {
        Vec2<uint16> position_in_vram; // Unit is 16-bit pixels
        Vec2<uint16> position_in_texture_page; // Unit is 16-bit pixels
    };
    

    class VramAllocator {
    public:

       
        VramAllocator() 
            :   global_allocator(2048, 512) // Bytes x Lines
        { 
            GridAllocator::Result result = global_allocator.allocate(
                320 * 2 * 2,
                240
            );

            frame_buffer_1_position.x = (uint16)(result.position.x >> 1);
            frame_buffer_1_position.y = (uint16)(result.position.y >> 1);

            frame_buffer_2_position.x = (uint16)(result.position.x >> 1) + 320;
            frame_buffer_2_position.y = (uint16)(result.position.y >> 1);
        }

        // Each pixel is 
        Vec2<uint16> allocate_clut_4() {
            GridAllocator::Result result = global_allocator.allocate(
                32,
                1
            );

            TD_ASSERT(result.success, "Failed to allocate 4-Bit CLUT (i.e. 32-bytes)");
            return { (uint16)(result.position.x >> 1), (uint16)result.position.y };
        }

        Vec2<uint16> allocate_clut_8() {
            GridAllocator::Result result = global_allocator.allocate(
                512,
                1
            );

            TD_ASSERT(result.success, "Failed to allocate 8-Bit CLUT (i.e. 512-bytes)");
            return { (uint16)(result.position.x >> 1), (uint16)result.position.y };
        }

        // Width in both argument and return value is in 16-byte units (so 1 is 4 pixels if mode == TextureMode::Palette4Bit)
        TextureAllocationResult allocate_texture(TextureMode mode, uint8 width, uint8 height) {
            for(uint32 i = 0; i < texture_pages.get_size(); i++ ) {
                TexturePage& texture_page = texture_pages[i];
                if( texture_page.mode != mode ) continue;

                GridAllocator::Result result = texture_page.allocator.allocate(width, height);
                if( result.success ) continue;

                return {
                    texture_page.position,
                    (Vec2<uint16>)result.position
                };
            }

            // No Texture Page can fit the texture, we create a new one
            uint16 num_16_bit_pixels_width =
                mode == TextureMode::TrueColor ? 256 :
                (mode == TextureMode::Palette8Bit ? 128 : 64);

            // No texture page available - we try and allocate new
            GridAllocator::Result texture_allocation = global_allocator.allocate(
                num_16_bit_pixels_width * 2,
                256,
                7,
                8
            );

            TD_ASSERT(texture_allocation.success, "Failed to allocate new texture page of mode %d", mode);
            texture_pages.add({
                { (uint16)(texture_allocation.position.x >> 1),(uint16)(texture_allocation.position.y) }, 
                num_16_bit_pixels_width,
                mode
            });

            TexturePage& new_page = texture_pages.get_last();
            GridAllocator::Result result_in_new_page = new_page.allocator.allocate(width, height);
            
            TD_ASSERT(result_in_new_page.success, "Failed to allocate texture of mode %d and size %dx%d in fresh texture page (texture is probably too big)", mode, width, height);
            
            return {
                new_page.position + (Vec2<uint16>)result_in_new_page.position,
                (Vec2<uint16>)result_in_new_page.position
            };
        }

        GridAllocator& get_global_allocator() { return global_allocator; }

        // TODO: Make these const
        Vec2<uint16> frame_buffer_1_position;
        Vec2<uint16> frame_buffer_2_position;

    private:

        class TexturePage {
        public:
            TexturePage(Vec2<uint16> position, uint16 width, TextureMode mode) 
                :   position(position), allocator(width, 256), mode(mode)
            { }

            Vec2<uint16> position;
            GridAllocator allocator;
            TextureMode mode;
        };
    
        GridAllocator global_allocator;

        td::List<TexturePage> texture_pages;

    };

}