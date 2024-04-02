#pragma once

#include <tundra/engine/entity-system/component.dec.hpp>

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td {

    template<typename TDerived, typename TBase>
    void Component<TDerived, TBase>::destroy() {
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

    template<typename TDerived, typename TBase>
    void Component<TDerived, TBase>::free() {
        internal::Registry<TDerived>::free_component(static_cast<TDerived*>(this));
    }

    template<typename TDerived, typename TBase>
    internal::Registry<TDerived>::Iterable Component<TDerived, TBase>::get_all() {
        return internal::Registry<TDerived>::get_all();
    }

}