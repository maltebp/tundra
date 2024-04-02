#include <tundra/rendering/primitive-buffer.hpp>

#include <tundra/core/assert.hpp>

namespace td {

    PrimitiveBuffer::PrimitiveBuffer(uint32 num_bytes)
        :   memory_start((byte*) ::operator new (num_bytes)),
            memory_end(memory_start + num_bytes),
            next_memory(memory_start)
    {
        TD_ASSERT(num_bytes > 0, "PrimitiveBuffer must be larger than 0");
        TD_ASSERT(memory_start != nullptr, "Failed to allocated memory of %d bytes", num_bytes);
    }

    PrimitiveBuffer::~PrimitiveBuffer() {
        if( memory_start != nullptr ) {
            ::operator delete(memory_start);
        }
    }

    void* PrimitiveBuffer::allocate(td::uint32 num_bytes) {
        byte* new_next = next_memory + num_bytes;
        
        if( new_next <= memory_end ) {
            // Not enough space to allocate that many bytes
            return nullptr;
        }
        else {
            byte* allocated_bytes = next_memory;
            next_memory = new_next;
            return allocated_bytes;
        }
    }

    void PrimitiveBuffer::clear() {
        next_memory = memory_start;
    }

    [[nodiscard]] uint32 PrimitiveBuffer::get_num_allocated_byte() const {
        return (uint32)(next_memory - memory_start);
    }

}