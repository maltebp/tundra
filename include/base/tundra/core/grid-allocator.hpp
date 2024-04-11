#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vec/vec2.dec.hpp>
#include <tundra/core/list.dec.hpp>

namespace td {

    // Currently it only supports allocated and not deallocating (so its more 
    // of a packer really)
    // OPTIMIZATION: This needs a more proper algorithm (right now, it is just for
    // allocating once at start-up)
    // TODO: Rename to Box or Allocator2D or similar (Grid sounds like it can store fixed sizes only)
    class GridAllocator {
    public:

        struct Result {
            bool success;
            Vec2<int16> position;
        };

        struct Rect {
            Vec2<int16> position;
            Vec2<int16> size;
            int32 area;
        };

        GridAllocator(uint16 width, uint16 height);

        // The actual byte alignment is 2^power
        [[nodiscard]] Result allocate(uint16 width, uint16 height, uint16 x_alignment_power = 0, uint16 y_alignment_power = 0);

        [[nodiscard]] uint32 get_num_allocations() const;

        [[nodiscard]] uint32 get_num_bytes_allocated() const;

        [[nodiscard]] const List<Rect>& get_free_rects() const;

        const uint16 width;
        const uint16 height;

    private:

        List<Rect> free_rects;

        uint32 num_allocations = 0;
        uint32 num_bytes_allocated = 0;

    };

}
