#pragma once

#include "tundra/engine/entity-system/internal/registry-block.dec.hpp"
#include <tundra/engine/entity-system/internal/registry.dec.hpp>

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/internal/component-meta-data.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/registry-block.hpp>

namespace td::internal {

    template<typename TComponent>
    template<typename ... TArgs>
    TComponent* Registry<TComponent>::create_component(TArgs&& ... args) {

        TComponent* component = get_free_block().allocate_component();
        new(component) TComponent(forward<TArgs>(args)...);

        component->flags |= ComponentFlags::IsAllocated | ComponentFlags::IsAlive; 
        component->reference_count = 0;
        component->next = component;
        
        return component;
    }

    template<typename TComponent>
    void Registry<TComponent>::destroy_component(TComponent* component) {
        component->destroy();
    }

    template<typename TComponent>
    void Registry<TComponent>::free_component(TComponent* component) {
        TD_ASSERT(
            component->reference_count == 0,
            "Component still has reference count of %d when freed",
            component->reference_count);

        TD_ASSERT( !component->is_alive(), "Component is still alive when freed");
        TD_ASSERT( component->is_allocated(), "Component has not been allocated when freed");
        
        RegistryBlock<TComponent>* owning_block = nullptr;
        for( uint32 i = 0; i < blocks.get_size(); i++ ) {
            if( blocks[i].component_belongs_to_block(component) ) {
                owning_block = &blocks[i];
                break;
            }
        }
        
        TD_ASSERT(owning_block != nullptr, "Component was not part of any block");
        component->~TComponent();
        owning_block->free_component(component);
    }   

    template<typename TComponent>
    void Registry<TComponent>::clear_block_list() {
        TD_ASSERT(get_num_allocated_components() == 0, "All components must be destroyed before clearing blocks");
        blocks.clear();
    }

    template<typename TComponent>
    uint32 Registry<TComponent>::get_num_allocated_components() { 
        // We can track this when components are created instead, if 
        // accumulating this when needed becomes a performance issue

        uint32 num_components = 0;
        for( uint32 i = 0; i < blocks.get_size(); i++ ) {
            num_components += blocks[i].get_num_allocated_components();
        }

        return num_components;
    }

    template<typename TComponent>
    uint32 Registry<TComponent>::get_num_blocks() { return blocks.get_size(); }

    template<typename TComponent>
    Registry<TComponent>::Iterable Registry<TComponent>::get_all() {
        return Registry<TComponent>::Iterable();
    }

    template<typename TComponent>   
    RegistryBlock<TComponent>& Registry<TComponent>::get_free_block() {
        for( uint32 i = 0; i < blocks.get_size(); i++ ) {
            RegistryBlock<TComponent>& block = blocks[i];
            if( block.has_free_entry() ) return block;
        }

        // No blocks are free, so we allocate new
        blocks.add(RegistryBlock<TComponent>(BLOCK_SIZE));
        return blocks.get_last();
    }

    template<typename TComponent>
    class Registry<TComponent>::Iterator {
    public:

        enum class Type { Begin, End };

        constexpr Iterator(Type type) {
            if( type == Type::End || Registry::get_num_blocks() == 0 ) {
                block_index = Registry::get_num_blocks();
                index_in_block = 0;
            }
            else {
                block_index = 0;
                index_in_block = 0;
                block = &Registry::blocks[0];
                if( !is_at_end_of_last_block() ) {
                    increment_while_at_invalid();
                }
            }
        }

        constexpr bool operator==(const Iterator& other) const {
            return block_index == other.block_index && index_in_block == other.index_in_block;
        }

        constexpr Iterator& operator++() {        

            ++index_in_block;
            increment_while_at_invalid(); 

            return *this;
        }

        constexpr TComponent* operator*() {
            TD_ASSERT(block_index < Registry::get_num_blocks(), "Dereferencing end iterator");
            return &block->entries[index_in_block];
        }

    private:

        bool is_at_end_of_last_block() const { return block_index >= Registry::get_num_blocks(); }

        void increment_while_at_invalid() {
            while( true ) {
                if( index_in_block >= block->capacity ) {
                    block_index++;   
                    index_in_block = 0;
                }

                if( block_index >= Registry::get_num_blocks() ) return;
                block = &Registry::blocks[block_index];
                
                if( block->entries[index_in_block].is_alive() ) return;

                if( !block->entries[index_in_block].is_allocated() ) {
                    index_in_block = block->entries[index_in_block].hole_data.index_to_opposite_end;
                }

                index_in_block++;
            }
        }

        uint32 block_index;
        uint32 index_in_block;
        RegistryBlock<TComponent>* block;

    };

    template<typename TComponent>
    Registry<TComponent>::Iterator Registry<TComponent>::Iterable::begin() {
        return Iterator(Iterator::Type::Begin);
    }

    template<typename TComponent>
    Registry<TComponent>::Iterator Registry<TComponent>::Iterable::end() {
        return Iterator(Iterator::Type::End);
    }


};