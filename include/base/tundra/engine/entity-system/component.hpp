#pragma once

#include "tundra/engine/entity-system/internal/component-flags.hpp"
#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-base.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td {

    class Entity;
    
    // TDerived must be deriving from Component<TDerived>
    template<typename TDerived>
    class Component : public internal::ComponentBase {
    public:
        
        virtual void destroy() override final {
            TD_ASSERT(is_alive(), "Component is not alive when destroyed");
            TD_ASSERT(is_allocated(), "Component is not allocated");
            
            ComponentBase* previous = get_previous_component();
            previous->next = next;

            this->flags &= ~internal::ComponentFlags::IsAlive;
    
            on_destroy();

            if( this->reference_count == 0 ) {
                free();
            }
        }

    protected:

        virtual void on_destroy() { }

        friend class Entity;

    private:
    
        virtual void free() override final {
            internal::Registry<TDerived>::free_component(static_cast<TDerived*>(this));
        }

    };

}