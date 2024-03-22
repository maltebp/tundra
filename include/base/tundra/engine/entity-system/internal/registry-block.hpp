#pragma once

#include <tundra/core/assert.hpp>
#include <tundra/core/limits.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>
#include <tundra/core/list.hpp>


namespace td::internal {

    // TODO: It might be beneficial to make this a non-template class, to limit
    // memory cost of instructions

    template<typename TComponent>
    class RegistryBlock {
    public:

        RegistryBlock(uint16 capacity) 
            :   capacity(capacity),
                entries((TComponent*) ::operator new(sizeof(TComponent) * capacity))
        {
            // There are some edge cases that I am not handling to support a block
            // of size 1, so we prevent it for the time being.
            TD_ASSERT(capacity > 1, "RegistryBlock capacity must be larger than 1");

            TD_ASSERT(
                entries != nullptr, 
                "Failed to allocate memory for entries RegistryBlock (%d entries of %d bytes, total %d bytes)",
                capacity,
                sizeof(TComponent),
                sizeof(TComponent) * capacity);

            force_reset();
        }

        RegistryBlock(const RegistryBlock&) = delete;

        RegistryBlock(RegistryBlock&& other) 
            :   capacity(other.capacity),
                entries(other.entries),
                hole_indices(td::move(other.hole_indices)),
                num_entries(other.num_entries)
        { 
            other.entries = nullptr;
        }

        ~RegistryBlock() {
            if( entries != nullptr ) ::operator delete ((void*)entries);
        }

        // This deallocates all entries and resets them - only call this if you
        // know you have relinquished all references to entries (mostly, if not only,
        //  used for testing)
        void force_reset() {
            
            for( uint16 i = 0; i < capacity; i++ ) {
                // We could probably get away with not setting most of these, but
                // it helps us catch more bugs.
                entries[i].flags = ComponentFlags::None;
            }

            // Set head to point to tail
            ComponentMetaData* hole_head = static_cast<ComponentMetaData*>(entries);
            hole_head->hole_index = capacity - 1;

            // Set tail to point to head
            ComponentMetaData* hole_tail = static_cast<ComponentMetaData*>(entries + capacity - 1);
            hole_tail->hole_index = 0;

            hole_indices.clear();
            hole_indices.add(0);
            num_entries = 0;
        }

        // Returns uninitialized memory block for TComponent
        TComponent* allocate_component() {
            TD_ASSERT(num_entries < capacity, "RegistryBlock is full (internal error)");

            // We use last holes first it is faster to delete holes this way
            uint16 head_index = hole_indices.get_last();

            TD_ASSERT(
                head_index < capacity, 
                "RegistryBlock's hole's head index was out of bounds (index was %d, but capacity is %d)",
                head_index, capacity);

            ComponentMetaData* hole_head = static_cast<ComponentMetaData*>(entries + head_index);

            uint16 tail_index = hole_head->hole_index;
        
            TD_ASSERT(
                tail_index < capacity, 
                "RegistryBlock's hole's tail index was out of bounds (index was %d, but capacity is %d)",
                tail_index, capacity);

            TComponent* hole_tail_entry = static_cast<TComponent*>(entries + hole_head->hole_index);

            // Adjust the hole
            ComponentMetaData* hole_tail = static_cast<ComponentMetaData*>(hole_tail_entry);
            bool head_is_last_in_hole = hole_head == hole_tail;
            if( head_is_last_in_hole ) {
                // There is no longer a hole, so remove it
                hole_indices.remove_at(hole_indices.get_size() - 1);
            }
            else {
                uint16 new_tail_index = tail_index - 1;

                TD_ASSERT(
                    new_tail_index < capacity, 
                    "RegistryBlock's hole's new tail index was out of bounds (index was %d, but capacity is %d)",
                    new_tail_index, capacity);
                
                ComponentMetaData* new_tail = static_cast<ComponentMetaData*>(entries + new_tail_index);
                new_tail->hole_index = head_index;
                hole_head->hole_index = new_tail_index;
            }            

            hole_tail_entry->flags |= ComponentFlags::IsAllocated;
            num_entries++;
            return hole_tail_entry;
        }

        void free_component(TComponent* component) {

            TD_ASSERT(component_belongs_to_block(component), "Component does not belong to block"); 

            ComponentMetaData* component_meta_data = static_cast<ComponentMetaData*>(component);
            TD_ASSERT(component_meta_data->is_allocated(), "Component to destroy is not allocated");

            bool component_is_first_entry = entries == component;
            bool component_is_last_entry = entries + capacity - 1 == component;

            // Warning: these entries may be out of allocated memory!
            ComponentMetaData* previous_entry = static_cast<ComponentMetaData*>(component - 1);
            ComponentMetaData* next_entry = static_cast<ComponentMetaData*>(component + 1);

            bool previous_entry_is_hole = !component_is_first_entry && !previous_entry->is_allocated();
            bool next_entry_is_hole = !component_is_last_entry && !next_entry->is_allocated();

            if( next_entry_is_hole && previous_entry_is_hole ) {
                // Merge the 2 holes
                ComponentMetaData* merged_hole_head = static_cast<ComponentMetaData*>(entries + previous_entry->hole_index);
                ComponentMetaData* merged_hole_tail = static_cast<ComponentMetaData*>(entries + next_entry->hole_index);

                merged_hole_head->hole_index = next_entry->hole_index;
                merged_hole_tail->hole_index = previous_entry->hole_index;

                // We can leave this, the next and previous entries untouched, 
                // because they now reside within the hole

                // I believe this is a bit faster than looking for the index in
                // the regular way and then deleting it
                for(uint32 i = 0; i < hole_indices.get_size(); i++ ) {
                    ComponentMetaData* hole_entry = static_cast<ComponentMetaData*>(entries + hole_indices[i]);
                    if( hole_entry == next_entry ) {
                        // Move last to new slot and delete the end (we don't care)
                        // about the order in this list
                        hole_indices[i] = hole_indices.get_last();
                        hole_indices.remove_at(hole_indices.get_size() - 1);
                        break;
                    }
                }
            }
            else if( previous_entry_is_hole ){
                // Append entry to existing hole
                ComponentMetaData* hole_head = static_cast<ComponentMetaData*>(entries + previous_entry->hole_index);
                hole_head->hole_index++;
                component_meta_data->hole_index = previous_entry->hole_index;
            }
            else if( next_entry_is_hole ){
                // Prepend entry to existing hole
                ComponentMetaData* hole_head = next_entry;
                ComponentMetaData* hole_tail = static_cast<ComponentMetaData*>(entries + hole_head->hole_index);
                
                uint16 tail_index = hole_head->hole_index;
                uint16 current_head_index = hole_tail->hole_index;
                uint16 new_head_index = current_head_index - 1;

                // Remove current head from list
                uint32 element_to_replace = hole_indices.index_of(current_head_index);
                TD_ASSERT(
                    element_to_replace < td::limits::numeric_limits<uint32>::max,
                    "RegistryBlock's hole starting at index %d was not found", 
                    hole_tail->hole_index);
                
                // Hole head is now the entry before (i.e. the one we just deleted)
                hole_indices[element_to_replace] = new_head_index;

                hole_tail->hole_index = new_head_index;
                component_meta_data->hole_index = tail_index;
            }
            else {
                // Add new hole
                component_meta_data->hole_index = get_index_of_component(component);
                hole_indices.add(component_meta_data->hole_index);
            }

            component_meta_data->flags = ComponentFlags::None;
            num_entries--;
            
        }

        [[nodiscard]] bool component_belongs_to_block(TComponent* component) const {
            return component >= entries && component < (entries + capacity);
        }

        [[nodiscard]] constexpr uint16 get_num_allocated_components() const { return num_entries; }

        [[nodiscard]] constexpr bool has_free_entry() const { return num_entries < capacity; }

        [[nodiscard]] constexpr uint16 get_num_holes() const { return (uint16)hole_indices.get_size(); }
    
        [[nodiscard]] uint16 get_index_of_component(TComponent* component) const {
            TD_ASSERT(component_belongs_to_block(component), "Component to get index of does not belong to block");

            // Messed this up: did not realize that subtracting pointers would
            // return the difference in indices (and not the byte difference)
            return (uint16)(component - entries);
        }

        class Iterator;

        Iterator begin();

        Iterator end();

    private:

        const uint16 capacity;
        TComponent* entries;
        td::List<uint16> hole_indices; // Indices of entries that are hole heads
        uint16 num_entries = 0;

        friend class RegistryBlockTester;

    };

    template<typename TComponent>
    class RegistryBlock<TComponent>::Iterator {
    public:

        enum class Type {
            Begin,
            End
        };

        // using iterator_category = std::forward_iterator_tag; // type of iterator
        // using difference_type = std::ptrdiff_t; // Unsure about this
        // using value_type = // value type
        // using pointer = // pointer type
        // using reference = // reference type

        constexpr Iterator() : block(nullptr), component(nullptr) { }

        constexpr Iterator(RegistryBlock* block, Type type) : block(block) {
            TD_ASSERT(block != nullptr, "Block must not be nullptr in non-default iterator constructor");
            if( type == Type::End || block->num_entries == 0) {
                component = block->entries + block->capacity;
            }
            else {
                component = block->entries;
                skip_if_hole();
            }
        }

        constexpr bool operator==(const Iterator& other) const {
            return &block == &other.block && component == other.component;
        } 

        constexpr Iterator& operator++() {
            component++;
            skip_if_hole();
            return *this;
        } 

        constexpr TComponent* operator*() {
            return component;
        } 

    private:

        constexpr void skip_if_hole() {
            TComponent* end_ptr = block->entries + block->capacity;

            if( component == end_ptr || component->is_allocated() ) return;
            uint16 hole_tail_index = component->hole_index;
            TComponent* hole_tail = block->entries + hole_tail_index;

            // This should either be the end poiner or a valid non-hole entry
            component = hole_tail + 1;
            TD_ASSERT(
                (component == end_ptr) || component->is_allocated(),
                "Entry after hole is neither the end pointer, nor an allocated (i.e. non-hole) entry"
            );
        }

        RegistryBlock* block;

        TComponent* component;

    };

    template<typename TComponent>
    RegistryBlock<TComponent>::Iterator RegistryBlock<TComponent>::begin() {
        return RegistryBlock<TComponent>::Iterator(this, Iterator::Type::Begin);
    }

    template<typename TComponent>
    RegistryBlock<TComponent>::Iterator RegistryBlock<TComponent>::end() {
        return RegistryBlock<TComponent>::Iterator(this, Iterator::Type::End);
    }

}