#pragma once

#include "tundra/core/log.hpp"
#include "tundra/engine/entity-system/internal/component-meta-data.hpp"
#include <tundra/core/list.dec.hpp>

namespace td::internal {

    // TODO: It might be beneficial to make this a non-template class, to limit
    // memory cost of instructions

    template<typename TComponent>
    class RegistryBlock {
    public:

        RegistryBlock(uint16 capacity);

        RegistryBlock(const RegistryBlock&) = delete;

        RegistryBlock(RegistryBlock&& other);

        ~RegistryBlock();

        // This deallocates all entries and resets them - only call this if you
        // know you have relinquished all references to entries (mostly, if not only,
        //  used for testing)
        void force_reset();

        // Returns uninitialized memory block for TComponent
        TComponent* allocate_component();

        void free_component(TComponent* component);

        [[nodiscard]] bool component_belongs_to_block(TComponent* component) const;

        [[nodiscard]] constexpr uint16 get_num_allocated_components() const;

        [[nodiscard]] constexpr bool has_free_entry() const;

        [[nodiscard]] constexpr uint16 get_num_holes() const;
    
        [[nodiscard]] uint16 get_index_of_component(TComponent* component) const;

        class Iterator;

        Iterator begin();

        Iterator end();

        // TODO: Delete this
        void print_meta_data() {
            TD_DEBUG_LOG("Meta data: ");
            for( uint16 i = 0; i < capacity; i++ ) {
                ComponentMetaData* meta_data = static_cast<ComponentMetaData*>(entries + i);
                TD_DEBUG_LOG("  i = %d, allocated = %s, hole_index = %d", i, meta_data->is_allocated() ? "true" : "false", meta_data->hole_index);
            }
        }

    private:

        const uint16 capacity;
        TComponent* entries;
        td::List<uint16> hole_indices; // Indices of entries that are hole heads
        uint16 num_entries = 0;

        friend class RegistryBlockTester;

    };

}