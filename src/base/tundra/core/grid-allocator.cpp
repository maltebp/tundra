#include <tundra/core/grid-allocator.hpp>

#include <tundra/core/limits.hpp>
#include <tundra/core/list.hpp>
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

    GridAllocator::Result GridAllocator::allocate(uint16 width, uint16 height, uint16 x_alignment_power, uint16 y_alignment_power) {
        const int16 x_alignment_mask = x_alignment_power == 0 ? 0 : ((int16)(1 << (x_alignment_power)) - 1);
        const int16 y_alignment_mask = y_alignment_power == 0 ? 0 : ((int16)(1 << (y_alignment_power)) - 1);

        Rect* smallest_rect = nullptr;
        uint32 smallest_rect_index = 0;

        auto is_rect_exact_aligned = [&](Rect& rect) {
            if( x_alignment_power != 0 ) {
                if( (rect.position.x & x_alignment_mask) != 0 ) return false;
            }
            if( y_alignment_power != 0 ) {
                if( (rect.position.y & y_alignment_mask) != 0 ) return false;
            }
            return true;
        };

        auto is_rect_better = [&](Rect& rect) {
             if( rect.size.x < width || rect.size.y < height ) return false;          

            if( !is_rect_exact_aligned(rect) ) {
                int16 next_aligned_x = (rect.position.x + x_alignment_mask) & ~x_alignment_mask;
                if( x_alignment_power != 0 && next_aligned_x + width > rect.position.x + rect.size.x) return false;

                int16 next_aligned_y = (rect.position.y + y_alignment_mask) & ~y_alignment_mask;
                if( y_alignment_power != 0 && next_aligned_y + height > rect.position.y + rect.size.y ) return false;

                // Rect can fit the adjusted value
            }

            if( smallest_rect == nullptr ) return true;

            if( rect.area > smallest_rect->area ) return false;
            if( rect.area < smallest_rect->area ) return true;

            if( is_rect_exact_aligned(rect) && !is_rect_exact_aligned(*smallest_rect) ) return true;

            return false;
        };
        
        for( uint32 i = 0; i < free_rects.get_size(); i++ ) {
            Rect& free_rect = free_rects[i];
            if( is_rect_better(free_rect) ) {
                smallest_rect = &free_rect;
                smallest_rect_index = i;
            }
        }

        if( smallest_rect == nullptr) return {false, {}};

        Rect& rect_to_split = *smallest_rect;

        const int16 aligned_alloc_x = (rect_to_split.position.x + x_alignment_mask) & ~x_alignment_mask;
        const int16 aligned_alloc_y = (rect_to_split.position.y + y_alignment_mask) & ~y_alignment_mask;
        const int16 alignment_padding_x = aligned_alloc_x - rect_to_split.position.x;
        const int16 alignment_padding_y = aligned_alloc_y - rect_to_split.position.y;
        const int16 aligned_width = (int16)width + alignment_padding_x;
        const int16 aligned_height = (int16)height + alignment_padding_y;

        // Note: we discard the alignment space for simplicity (could be optimized so we don't)

        if( aligned_width == rect_to_split.size.x && aligned_height == rect_to_split.size.y ) {
            free_rects.remove_at(smallest_rect_index);
        }
        else if( aligned_width == rect_to_split.size.x ) {
            // Fills entire width, so only create one new rect above (this does not have to be aligned)
            Rect new_rect_above;
            new_rect_above.position = { rect_to_split.position.x, (int16)(rect_to_split.size.y + aligned_height)};
            new_rect_above.size = { rect_to_split.size.x, (int16)(rect_to_split.size.y - aligned_height)};
            new_rect_above.area = new_rect_above.size.x * new_rect_above.size.y;
            free_rects[smallest_rect_index] = new_rect_above;

        }
        else if( aligned_height == rect_to_split.size.y ) {
            Rect new_rect_right;
            new_rect_right.position = { (int16)(rect_to_split.position.x + aligned_width), rect_to_split.position.y};
            new_rect_right.size = { 
                (int16)(rect_to_split.size.x - aligned_width),
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
            new_rect_above.position = { rect_to_split.position.x, (int16)(rect_to_split.position.y + aligned_height) };
            new_rect_above.size = { rect_to_split.size.x, (int16)(rect_to_split.size.y - aligned_height)};
            new_rect_above.area = new_rect_above.size.x * new_rect_above.size.y;

            Rect new_rect_right;
            new_rect_right.position = { (int16)(rect_to_split.position.x + aligned_width), rect_to_split.position.y };
            new_rect_right.size = { 
                (int16)(rect_to_split.size.x - aligned_width),
                aligned_height
            };
            new_rect_right.area = new_rect_right.size.x * new_rect_right.size.y;
            
            free_rects[smallest_rect_index] = new_rect_above;
            free_rects.add(new_rect_right);   
        }

        num_allocations++;
        num_bytes_allocated += (uint32)(aligned_width * aligned_height);
        return { true, { aligned_alloc_x, aligned_alloc_y } };
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