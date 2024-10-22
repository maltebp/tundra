#pragma once

#include <tundra/engine/entity-system/internal/registry-block.dec.hpp>

namespace td::internal {

    // TODO: Rename to ComponentRegistry 
    // TODO: Registry should be refactored to a non-static class instead of this approach
    template<typename TComponent>
    class Registry {    
    public:

        class Iterator;

        struct Iterable {
            Iterator begin();
            Iterator end();
        };

        template<typename ... TArgs>
        static TComponent* create_component(TArgs&& ... args);

        static void destroy_component(TComponent* component);

        static void free_component(TComponent* component);

        // This deallocates all blocks (and thus deallocates all components), BUT
        // it does not destroy the components.
        // Only use this for testing and if you know what you are doing
        static void clear_block_list();

        static void reserve(td::uint32);

        // Returns the number of alive components + the number dead components
        // that have not been freed yet
        static uint32 get_num_allocated_components();

        static uint32 get_num_blocks();

        static uint32 get_capacity();
        
        static Iterable get_all();

        // TODO: Make this tweakable by user (this number is pulled out of a hat)
        static inline constexpr uint32 BLOCK_SIZE = 25; 
        
    private:

        static RegistryBlock<TComponent>& get_free_block();

        static RegistryBlock<TComponent>& allocate_block();

        static inline List<RegistryBlock<TComponent>> blocks;
        static inline List<typename RegistryBlock<TComponent>::BlockIndex> free_blocks;

    };

};