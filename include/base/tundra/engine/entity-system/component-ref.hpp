#pragma once

#include <tundra/core/assert.hpp>
#include <tundra/core/string.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>

// TODO: Create a component-ref.dec.hpp

namespace td {

    template<typename TComponent>
    class ComponentRef {
    public:

        ComponentRef() = default;

        ComponentRef(TComponent* component) {
            if( component != nullptr && component->is_alive() ) {
                this->component = component;
                component->reference_count++;
            }
        }

        ComponentRef(const ComponentRef& other) {
            if( other.component != nullptr && other.component->is_alive() ) {
                component = other.component;
                component->reference_count++;
            }
        }

        ComponentRef(ComponentRef&& other) {
            if( other.component != nullptr && other.component->is_alive() ) {
                component = other.component;
                other.component = nullptr;
            }
        }

        ~ComponentRef() {
            clear();
        }

        void operator=(const ComponentRef<TComponent>& other) {
            other.clear_if_dead();
            clear();
            this->component = other.component;
            if( component != nullptr ) {
                component->reference_count++;
            }
        }

        void operator=(TComponent* component) {
            TD_ASSERT(component == nullptr || component->is_alive(), "Non-nullptr component is not alive");
        
            clear();
            this->component = component;
            if( component != nullptr ) {
                component->reference_count++;
            }
        }

        [[nodiscard]] constexpr bool operator==(TComponent* other) const {
            clear_if_dead();
            return component == other;
        }

        [[nodiscard]] constexpr bool operator==(const ComponentRef& other) const {
            clear_if_dead();
            other.clear_if_dead();
            return component == other.component;
        }

        [[nodiscard]] TComponent* operator->() const {
            clear_if_dead();
            return component;
        }

        [[nodiscard]] operator TComponent*() const {
            clear_if_dead();
            return component;
        }

        void clear_if_dead() const {
            if( component != nullptr && !component->is_alive()) {
                clear();
            }
        }

    private:
        
        void clear() const {
            if( component == nullptr ) return;

            TD_ASSERT(
                component->is_allocated(), 
                "Referenced component has been freed before reference was cleared");

            component->reference_count--;
            if( component->reference_count == 0 && !component->is_alive() ) {
                internal::Registry<TComponent>::free_component(component);
            }

            component = nullptr;
        }
        
        mutable TComponent* component = nullptr;       

    };

    using EntityRef = ComponentRef<Entity>;

    template<typename TComponent>
    String to_string(const ComponentRef<TComponent>& ref) {
        return to_string((const TComponent*)ref);
    }

}