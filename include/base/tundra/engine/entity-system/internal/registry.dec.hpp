#pragma once

#include <tundra/engine/entity-system/internal/registry-block.dec.hpp>

namespace td::internal {

    // TODO: Rename to ComponentRegistry 

    template<typename TComponent>
    class Registry {    
    public:

        class Iterator;

        struct Iterable {
            Iterator begin();
            Iterator end();
        };

        // TODO: Non-default create function

        template<typename ... TArgs>
        static TComponent* create_component(TArgs&& ... args);

        // TODO: Exists because I was too lazy to refactor (refactor this)
        static void destroy_component(TComponent* component);

        static void free_component(TComponent* component);

        // This deallocates all blocks (and thus deallocates all components), BUT
        // it does not destroy the components.
        // Only use this for testing and if you know what you are doing
        static void clear_block_list();

        // Returns the number of alive components + the number dead components
        // that have not been freed yet
        static uint32 get_num_allocated_components();

        static uint32 get_num_blocks();

        // TODO: Registry should be refactored to a non-static class instead of this approach
        // This was a quick hack to allow us to iterator static registry in a foreach loop
        
        static Iterable get_all();

        static void for_each(void(*callback)(TComponent* t)) {
            for( td::uint32 i = 0; i < blocks.get_size(); i++ ) {
                RegistryBlock<TComponent>& block = blocks[i];
                for( td::uint16 j = 0; j < block.capacity; j++ ) {
                    TComponent* entry = &block.entries[j];
                    // if( !entry->is_allocated() ) {
                    //     j = entry->hole_index + 1;
                    // }
                    // else 
                    if( entry->is_alive() ) {
                        callback(entry);
                    }
                }
            }
        }

        // TODO: Make this tweakable by user (this number is pulled out of a hat)
        static inline constexpr uint32 BLOCK_SIZE = 25; 
        
    private:

        static RegistryBlock<TComponent>& get_free_block();

        static inline List<RegistryBlock<TComponent>> blocks;

    };

};