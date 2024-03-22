#pragma once

#include <cstddef>
#include <tundra/core/assert.hpp>
#include <tundra/core/string.hpp>

#include <tundra/engine/entity-system/internal/registry.hpp>

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

        void operator=(TComponent* component) {
            TD_ASSERT(component->is_alive(), "Component is not alive");
            
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

        // TODO: This should just return a pointer - you have to type out the same
        // things anyway. And in that case we can just use the defereference operator

        [[nodiscard]] bool get(TComponent*& out) {
            clear_if_dead();
            out = component;
            return component == nullptr;
        }

        [[nodiscard]] bool get(TComponent const *& out) const {
            clear_if_dead();
            out = component;
            return component == nullptr;
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
        const TComponent* component;
        if( ref.get(component) ) {
            return to_string(component);
        }
        else {
            return to_string(nullptr);
        }
    }

}