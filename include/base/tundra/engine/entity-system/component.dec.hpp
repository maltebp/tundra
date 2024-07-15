#pragma once

#include <tundra/engine/entity-system/internal/component-base.hpp>
#include <tundra/engine/entity-system/entity.fwd.hpp>
#include <tundra/engine/entity-system/internal/registry.dec.hpp>

namespace td {

    // TDerived must be deriving from Component<TDerived>
    template<typename TDerived, typename TBase = internal::ComponentBase>
    class Component : public TBase {
    public:
        
        virtual void destroy() override final;

        static internal::Registry<TDerived>::Iterable get_all();

        // Ensures that there is allocated room for at least num_components
        static void reserve(td::uint32 num_components);

        static uint32 get_total_data_memory();

    private:
    
        virtual void free() override final;

        friend class Entity;

    };

}