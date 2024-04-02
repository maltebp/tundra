#pragma once

#include <tundra/core/types.hpp>

namespace td {

    // TODO: I want to allow this buffer to grow, by allocating new blocks (i.e 
    // not re-allocated existing memory)

    class PrimitiveBuffer { 
    public:

        PrimitiveBuffer(uint32 num_bytes);

        PrimitiveBuffer(const PrimitiveBuffer&) = delete;

        ~PrimitiveBuffer();

        [[nodiscard]] void* allocate(td::uint32 num_bytes);

        void clear();

        uint32 get_num_allocated_byte() const;

    private:

        byte* const memory_start;
        byte* const memory_end;
        byte* next_memory;
    };

}