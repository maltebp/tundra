#pragma once

#include <tundra/engine/entity-system/entity.fwd.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>

namespace td {
    template<typename T>
    class ComponentRef;
    
    template<typename TDerived, typename TBase>
    class Component;
}

namespace td::internal {

    class ComponentBase;

    class ComponentMetaData {
    public:

        struct HoleData {
            
            // If this is the hole head, this is the index of the hole tail, and
            // if it is the tail, it is the index of the hole head.
            uint16 index_to_opposite_end;

            uint16 index_in_hole_list;
        };

        [[nodiscard]] bool is_alive() const { 
            return (bool)(flags & ComponentFlags::IsAlive);
        }

        [[nodiscard]] bool is_allocated() const { 
            return (bool)(flags & ComponentFlags::IsAllocated);
        }

        [[nodiscard]] bool is_entity() const { 
            return (bool)(flags & ComponentFlags::IsEntity);
        }

    private:

        ComponentFlags flags;
        uint8_t reference_count;
        uint8_t block_index;
        
        union {
            ComponentBase* next;
            HoleData hole_data;
        };

        template<typename TComponent>
        friend class RegistryBlock;

        template<typename TComponent>
        friend class Registry;

        friend class td::Entity;

        template<typename T>
        friend class td::ComponentRef;

        friend class ComponentBase;

        template<typename TDerived, typename TBase>
        friend class td::Component;

    };

}