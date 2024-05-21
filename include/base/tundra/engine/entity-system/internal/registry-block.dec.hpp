#pragma once

#include <tundra/core/list.dec.hpp>

namespace td::internal {

    // TODO: It might be beneficial to make this a non-template class, to limit
    // memory cost of instructions

    template<typename T>
    class Registry;

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

    private:

        const uint16 capacity;
        TComponent* entries;
        td::List<uint16> hole_indices; // Indices of entries that are hole heads
        uint16 num_entries = 0;

        friend class RegistryBlockTester;
        friend class ::td::internal::Registry<TComponent>;

    };

}