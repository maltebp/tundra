#pragma once

#include <tundra/engine/entity-system/component.dec.hpp>

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td {

    template<typename TDerived, typename TBase>
    uint32 Component<TDerived, TBase>::get_total_data_memory() {
        return internal::Registry<TDerived>::get_num_allocated_components() * (sizeof(TDerived) - sizeof(Component<TDerived, TBase>));
    }

    template<typename TDerived, typename TBase>
    void Component<TDerived, TBase>::destroy() {
        TD_ASSERT(TBase::is_alive(), "Component is not alive when destroyed");
        TD_ASSERT(TBase::is_allocated(), "Component is not allocated");           

        this->on_destroy();

        internal::ComponentBase* previous = this->get_previous_component();
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

    template<typename TDerived, typename TBase>
    void Component<TDerived, TBase>::reserve(td::uint32 num_components) {
        internal::Registry<TDerived>::reserve(num_components);
    }

}