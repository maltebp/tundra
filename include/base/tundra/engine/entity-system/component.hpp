#pragma once

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-base.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td {

    class Entity;
    
    // TDerived must be deriving from Component<TDerived>
    template<typename TDerived, typename TBase = internal::ComponentBase>
    class Component : public TBase {
    public:
        
        virtual void destroy() override final {
            TD_ASSERT(TBase::is_alive(), "Component is not alive when destroyed");
            TD_ASSERT(TBase::is_allocated(), "Component is not allocated");           
    
            on_destroy();

            internal::ComponentBase* previous = TBase::get_previous_component();
            previous->next = TBase::next;
            this->flags &= ~internal::ComponentFlags::IsAlive;

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