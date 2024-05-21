#pragma once

#include <tundra/engine/entity-system/internal/registry-block.dec.hpp>

#include <tundra/core/assert.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>
#include <tundra/core/list.hpp>

namespace td::internal {


    template<typename TComponent>
    RegistryBlock<TComponent>::RegistryBlock(uint16 capacity) 
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

    template<typename TComponent>
    RegistryBlock<TComponent>::RegistryBlock(RegistryBlock&& other) 
        :   capacity(other.capacity),
            entries(other.entries),
            hole_indices(td::move(other.hole_indices)),
            num_entries(other.num_entries)
    { 
        other.entries = nullptr;
    }

    template<typename TComponent>
    RegistryBlock<TComponent>::~RegistryBlock() {
        if( entries != nullptr ) ::operator delete ((void*)entries);
    }

    template<typename TComponent>
    void RegistryBlock<TComponent>::force_reset() {
        
        for( uint16 i = 0; i < capacity; i++ ) {
            // We could probably get away with not setting most of these, but
            // it helps us catch more bugs.
            entries[i].flags = ComponentFlags::None;
        }

        // Set head to point to tail
        ComponentMetaData* hole_head = static_cast<ComponentMetaData*>(entries);
        hole_head->hole_data.index_to_opposite_end = capacity - 1;

        // Set tail to point to head
        ComponentMetaData* hole_tail = static_cast<ComponentMetaData*>(entries + capacity - 1);
        hole_tail->hole_data.index_to_opposite_end = 0;

        hole_indices.clear();
        hole_indices.add(0);
        num_entries = 0;
    }

    template<typename TComponent>
    TComponent* RegistryBlock<TComponent>::allocate_component() {
        TD_ASSERT(num_entries < capacity, "RegistryBlock is full (internal error)");

        // We use last holes first it is faster to delete holes this way
        uint16 head_index = hole_indices.get_last();

        TD_ASSERT(
            head_index < capacity, 
            "RegistryBlock's hole's head index was out of bounds (index was %d, but capacity is %d)",
            head_index, capacity);

        ComponentMetaData* hole_head = static_cast<ComponentMetaData*>(entries + head_index);

        uint16 tail_index = hole_head->hole_data.index_to_opposite_end;
    
        TD_ASSERT(
            tail_index < capacity, 
            "RegistryBlock's hole's tail index was out of bounds (index was %d, but capacity is %d)",
            tail_index, capacity);

        TComponent* hole_tail_entry = static_cast<TComponent*>(entries + hole_head->hole_data.index_to_opposite_end);

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
            new_tail->hole_data.index_to_opposite_end = head_index;
            hole_head->hole_data.index_to_opposite_end = new_tail_index;
        }            

        hole_tail_entry->flags |= ComponentFlags::IsAllocated;
        num_entries++;
        return hole_tail_entry;
    }

    template<typename TComponent>
    void RegistryBlock<TComponent>::free_component(TComponent* component) {

        TD_ASSERT(component_belongs_to_block(component), "Component does not belong to block"); 

        ComponentMetaData* component_meta_data = static_cast<ComponentMetaData*>(component);
        TD_ASSERT(component_meta_data->is_allocated(), "Component to destroy is not allocated");

        bool component_is_first_entry = component == entries;
        bool component_is_last_entry = component == entries + (capacity - 1);

        // Warning: these entries may be out of allocated memory!
        ComponentMetaData* previous_entry = static_cast<ComponentMetaData*>(component - 1);
        ComponentMetaData* next_entry = static_cast<ComponentMetaData*>(component + 1);

        bool previous_entry_is_hole = !component_is_first_entry && !previous_entry->is_allocated();
        bool next_entry_is_hole = !component_is_last_entry && !next_entry->is_allocated();

        if( next_entry_is_hole && previous_entry_is_hole ) {

            uint16 merged_hole_head_index = previous_entry->hole_data.index_to_opposite_end;
            uint16 merged_hole_tail_index = next_entry->hole_data.index_to_opposite_end;

            // Merge the 2 holes
            ComponentMetaData* merged_hole_head = static_cast<ComponentMetaData*>(entries + merged_hole_head_index);
            ComponentMetaData* merged_hole_tail = static_cast<ComponentMetaData*>(entries + merged_hole_tail_index);

            merged_hole_head->hole_data.index_to_opposite_end = merged_hole_tail_index;
            merged_hole_tail->hole_data.index_to_opposite_end = merged_hole_head_index;

            // Remove the next entry from the hole list
            uint16 last_hole_index = hole_indices.get_last();
            ComponentMetaData* last_hole = entries + hole_indices.get_last();
            last_hole->hole_data.index_in_hole_list = next_entry->hole_data.index_in_hole_list;
            hole_indices[next_entry->hole_data.index_in_hole_list] = last_hole_index;
            hole_indices.remove_at(hole_indices.get_size() - 1);
        }
        else if( previous_entry_is_hole ){
            // Append entry to existing hole
            ComponentMetaData* previous_hole_tail = previous_entry;
            uint16 hole_head_index = previous_hole_tail->hole_data.index_to_opposite_end;
            ComponentMetaData* previous_hole_head = static_cast<ComponentMetaData*>(entries + hole_head_index);

            // Set the hole's tail index to point to next entry (the component we just freed)
            previous_hole_head->hole_data.index_to_opposite_end++;

            component_meta_data->hole_data.index_to_opposite_end = hole_head_index;
        }
        else if( next_entry_is_hole ){
            // Prepend entry to existing hole
            ComponentMetaData* hole_head = next_entry;
            ComponentMetaData* hole_tail = static_cast<ComponentMetaData*>(entries + hole_head->hole_data.index_to_opposite_end);
            
            uint16 tail_index = hole_head->hole_data.index_to_opposite_end;
            uint16 current_head_index = hole_tail->hole_data.index_to_opposite_end;
            uint16 new_head_index = current_head_index - 1;

            // Remove current head from list
            uint32 hole_index_in_hole_list = hole_head->hole_data.index_in_hole_list;
            TD_ASSERT(
                hole_index_in_hole_list < td::limits::numeric_limits<uint32>::max,
                "RegistryBlock's hole starting at index %d was not found", 
                current_head_index);
            
            // Hole head is now the entry before (i.e. the one we just deleted)
            hole_indices[hole_index_in_hole_list] = new_head_index;

            hole_tail->hole_data.index_to_opposite_end = new_head_index;
            component_meta_data->hole_data.index_to_opposite_end = tail_index;
            component_meta_data->hole_data.index_in_hole_list = hole_head->hole_data.index_in_hole_list;
        }
        else {
            // Add new hole
            component_meta_data->hole_data.index_to_opposite_end = get_index_of_component(component);
            component_meta_data->hole_data.index_in_hole_list = (uint16)hole_indices.add(
                component_meta_data->hole_data.index_to_opposite_end
            );
        }

        component_meta_data->flags = ComponentFlags::None;
        num_entries--;
        
    }

    template<typename TComponent>
    [[nodiscard]] bool RegistryBlock<TComponent>::component_belongs_to_block(TComponent* component) const {
        return component >= entries && component < (entries + capacity);
    }

    template<typename TComponent>
    [[nodiscard]] constexpr uint16 RegistryBlock<TComponent>::get_num_allocated_components() const { return num_entries; }

    template<typename TComponent>
    [[nodiscard]] constexpr bool RegistryBlock<TComponent>::has_free_entry() const { return num_entries < capacity; }

    template<typename TComponent>
    [[nodiscard]] constexpr uint16 RegistryBlock<TComponent>::get_num_holes() const { return (uint16)hole_indices.get_size(); }

    template<typename TComponent>
    [[nodiscard]] uint16 RegistryBlock<TComponent>::get_index_of_component(TComponent* component) const {
        TD_ASSERT(component_belongs_to_block(component), "Component to get index of does not belong to block");

        // Messed this up: did not realize that subtracting pointers would
        // return the difference in indices (and not the byte difference)
        return (uint16)(component - entries);
    }
}