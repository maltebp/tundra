#pragma once

#include <tundra/core/assert.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/registry-block.hpp>

namespace td::internal {

    // TODO: Rename to ComponentRegistry 

    template<typename TComponent>
    class Registry {    
    public:

        // TODO: Non-default create function

        static TComponent* create_component() {

            TComponent* component = get_free_block().allocate_component();
            new(component) TComponent();

            component->flags |= ComponentFlags::IsAllocated | ComponentFlags::IsAlive; 
            component->reference_count = 0;
            component->next = component;
            
            return component;
        }

        // TODO: Exists because I was too lazy to refactor (refactor this)
        static void destroy_component(TComponent* component) {
            component->destroy();
        }

        static void free_component(TComponent* component) {
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

        // This deallocates all blocks (and thus deallocates all components), BUT
        // it does not destroy the components.
        // Only use this for testing and if you know what you are doing
        static void clear_block_list() {
            TD_ASSERT(get_num_components() == 0, "All components must be destroyed before clearing blocks");
            blocks.clear();
        }

        static uint32 get_num_components() { 
            // We can track this when components are created instead, if 
            // accumulating this when needed becomes a performance issue

            uint32 num_components = 0;
            for( uint32 i = 0; i < blocks.get_size(); i++ ) {
                num_components += blocks[i].get_num_allocated_components();
            }

            return num_components;
        }

        static uint32 get_num_blocks() { return blocks.get_size(); }

        class Iterator;

        // TODO: Registry should be refactored to a non-static class instead of this approach
        // This was a quick hack to allow us to iterator static registry in a foreach loop
        struct Iterable {
            Iterator begin();
            Iterator end();
        };

        static Iterable get_iterable() {
            return {};
        }

        // TODO: Make this tweakable by user (this number is pulled out of a hat)
        static inline constexpr uint32 BLOCK_SIZE = 25; 
        
    private:

        static RegistryBlock<TComponent>& get_free_block() {
            for( uint32 i = 0; i < blocks.get_size(); i++ ) {
                RegistryBlock<TComponent>& block = blocks[i];
                if( block.has_free_entry() ) return block;
            }

            // No blocks are free, so we allocate new
            blocks.add(RegistryBlock<TComponent>(BLOCK_SIZE));
            return blocks.get_last();
        }

        static inline List<RegistryBlock<TComponent>> blocks;

    };


    template<typename TComponent>
    class Registry<TComponent>::Iterator {
    public:

        using BlockIterator = RegistryBlock<TComponent>::Iterator;

        enum class Type { Begin, End };

        constexpr Iterator(Type type) {
            if( type == Type::End || Registry::get_num_blocks() == 0 ) {
                block_index = Registry::get_num_blocks();
                block_iterator = {};
            }
            else {
                block_index = 0;
                block_iterator = Registry::blocks[0].begin();
                skip_if_hole_or_block_end();
            }
        }

        constexpr bool operator==(const Iterator& other) const {
            
            if( block_index >= Registry::get_num_blocks() ) {
                return other.block_index >= Registry::get_num_blocks();
            }
            else {
                return block_index == other.block_index && block_iterator == other.block_iterator;
            }
        }

        constexpr Iterator& operator++() {
            if( block_index >= Registry::get_num_blocks() ) return *this;

            TD_ASSERT(
                block_iterator != BlockIterator{},
                "Block index is less than number of blocks, but iterator is nullptr"
            );

            ++block_iterator;
            skip_if_hole_or_block_end();
            return *this;
        }

        constexpr TComponent* operator*() {
            TD_ASSERT(block_index < Registry::get_num_blocks(), "Dereferencing end iterator");
            return *block_iterator;
        }

    private:

        void skip_if_hole_or_block_end() {

            if( block_index >= Registry::get_num_blocks() ) return;

            bool reached_end_of_block = block_iterator == Registry::blocks[block_index].end();
            if( !reached_end_of_block ) return;

            // Find next, non-empty block
            block_index++;
            while(block_index < Registry::get_num_blocks()) {
                RegistryBlock<TComponent>& block = Registry::blocks[block_index];
                if( block.get_num_allocated_components() > 0 ) {
                    block_iterator = block.begin();
                    break;
                }
                else {
                    block_index++;
                }
            }
        }

        uint32 block_index;

        BlockIterator block_iterator;

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