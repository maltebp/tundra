#include "tundra/core/assert.hpp"
#include <tundra/core/grid-allocator.hpp>

#include <tundra/core/limits.hpp>
#include <tundra/core/vec/vec2.hpp>

namespace td {

    GridAllocator::GridAllocator(uint16 width, uint16 height) 
        :   width(width),
            height(height) 
    {
        free_rects.add(Rect{
            {0, 0}, {(int16)width, (int16)height}, (int16)(width*height)
        });
    }

    GridAllocator::Result GridAllocator::allocate(uint16 width, uint16 height) {

        int32 smallest_rect_area = td::limits::numeric_limits<int32>::max;
        uint32 smallest_rect_index = 0;
        
        for( uint32 i = 0; i < free_rects.get_size(); i++ ) {
            Rect& free_rect = free_rects[i];
            
            if( free_rect.size.x < width || free_rect.size.y < height ) continue;
            if( free_rect.area >= smallest_rect_area ) continue;
            smallest_rect_area = 0;
            smallest_rect_index = i;
        }

        if( smallest_rect_area == td::limits::numeric_limits<int32>::max ) return {false, {}};

        Rect rect_to_split = free_rects[smallest_rect_index];
        

        if( width == rect_to_split.size.x && height == rect_to_split.size.y ) {
            free_rects.remove_at(smallest_rect_index);
        }
        else if( width == rect_to_split.size.x ) {
            Rect new_rect_above;
            new_rect_above.position = { rect_to_split.position.x, (int16)(rect_to_split.position.y + (int16)height)};
            new_rect_above.size = { rect_to_split.size.x, (int16)(rect_to_split.size.y - (int16)height)};
            new_rect_above.area = new_rect_above.size.x * new_rect_above.size.y;
            free_rects[smallest_rect_index] = new_rect_above;

        }
        else if( height == rect_to_split.size.y ) {
            Rect new_rect_right;
            new_rect_right.position = { (int16)(rect_to_split.position.x + (int16)width), rect_to_split.position.y};
            new_rect_right.size = { 
                (int16)(rect_to_split.size.x - (int16)width),
                 rect_to_split.size.y
            };
            new_rect_right.area = new_rect_right.size.x * new_rect_right.size.y;
            free_rects[smallest_rect_index] = new_rect_right;
        } 
        else {
            // We split it like
            // 
            //  _____________
            // | above       |
            // | _ _ _ __ _ _|
            // | allo |right |
            // |______|______|

            Rect new_rect_above;
            new_rect_above.position = { rect_to_split.position.x, (int16)(rect_to_split.position.y + (int16)height)};
            new_rect_above.size = { rect_to_split.size.x, (int16)(rect_to_split.size.y - (int16)height)};
            new_rect_above.area = new_rect_above.size.x * new_rect_above.size.y;

            Rect new_rect_right;
            new_rect_right.position = { (int16)(rect_to_split.position.x + (int16)width), rect_to_split.position.y};
            new_rect_right.size = { 
                (int16)(rect_to_split.size.x - (int16)width),
                (int16)height
            };
            new_rect_right.area = new_rect_right.size.x * new_rect_right.size.y;
            
            free_rects[smallest_rect_index] = new_rect_above;
            free_rects.add(new_rect_right);   
        }

        num_allocations++;
        num_bytes_allocated += width * height;
        return { true, rect_to_split.position };
    }

    const List<GridAllocator::Rect>& GridAllocator::get_free_rects() const {
         return free_rects;
    }

    uint32 GridAllocator::get_num_allocations() const {
        return num_allocations;
    }

    uint32 GridAllocator::get_num_bytes_allocated() const {
        return num_bytes_allocated;
    }   

}