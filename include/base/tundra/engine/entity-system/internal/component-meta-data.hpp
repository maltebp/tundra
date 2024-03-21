#pragma once

#include <tundra/engine/entity-system/entity.fwd.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>

namespace td {
    template<typename T>
    class ComponentRef;
    
    template<typename T>
    class Component;
}

namespace td::internal {

    class ComponentBase;

    class ComponentMetaData {
    public:

        [[nodiscard]] bool is_allocated() const;

        [[nodiscard]] bool is_alive() const;

        [[nodiscard]] bool is_entity() const;

    private:

        ComponentFlags flags;
        uint8_t reference_count;
        
        union {
            ComponentBase* next; // TODO: Should this be a pointer to a component?
            uint16 hole_index;
        };

        template<typename TComponent>
        friend class RegistryBlock;

        template<typename TComponent>
        friend class Registry;

        friend class td::Entity;

        template<typename T>
        friend class td::ComponentRef;

        friend class ComponentBase;

        template<typename T>
        friend class td::Component;

    };

}