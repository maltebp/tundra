#pragma once

#include <tundra/engine/entity-system/entity.fwd.hpp>
#include <tundra/engine/entity-system/internal/component-base.hpp>

namespace td {

    // TDerived must be deriving from Component<TDerived>
    template<typename TDerived, typename TBase = internal::ComponentBase>
    class Component : public TBase {
    public:
        
        virtual void destroy() override final;

    protected:

        virtual void on_destroy() { }

    private:
    
        virtual void free() override final;

        friend class Entity;

    };

}