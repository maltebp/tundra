#pragma once

#include <tundra/core/assert.hpp>

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
            if( other.component != nullptr && other.component.is_alive() ) {
                component = other.component;
                component->reference_count++;
            }
        }

        ComponentRef(ComponentRef&& other) {
            if( other.component != nullptr && other.component.is_alive() ) {
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
            component->reference_count++;
        }

        bool get(TComponent*& out) {
            if( component == nullptr ) {
                out = nullptr;
                return false;
            } 
            else if( !component->is_alive() ) {
                clear();
                out = nullptr;
                return false;
            }
            else {
                out = component;
                return true;
            }
        }

        bool get(TComponent const *& out) const {
            if( component == nullptr ) {
                out = nullptr;
                return false;
            } 
            else if( !component->is_alive() ) {
                clear();
                out = nullptr;
                return false;
            }
            else {
                out = component;
                return true;
            }
        }

        void release_if_dead() const {
            clear();
        }

    private:
        
        void clear() {
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

}