#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/list.dec.hpp>
#include <tundra/core/vec/vec2.dec.hpp>
#include <tundra/core/grid-allocator.hpp>
#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

     struct TextureAllocationResult {
        Vec2<uint16> position_in_vram; // Unit is 16-bit pixels
        Vec2<uint16> position_in_texture_page; // Unit is 16-bit pixels
    };

    class VramAllocator {
    public:
       
        VramAllocator();

        // Each pixel is 2 colors
        Vec2<uint16> allocate_clut_4(); 

        Vec2<uint16> allocate_clut_8();

        // Width in both argument and return value is in 16-byte units (so 1 is 4 pixels if mode == TextureMode::Palette4Bit)
        TextureAllocationResult allocate_texture(TextureMode mode, uint8 width, uint8 height);

        GridAllocator& get_global_allocator();

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