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

        // TODO: This should be adjustable on a type-level

        static inline List<RegistryBlock<TComponent>> blocks;

    };

};