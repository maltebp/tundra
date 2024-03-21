#pragma once

#include <tundra/engine/entity-system/internal/component-base.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td {

    class Entity : public internal::ComponentBase {
    public:

        [[nodiscard]] static Entity* create() {
            Entity* entity = internal::Registry<Entity>::create_component();
            entity->flags |= internal::ComponentFlags::IsEntity;
            entity->next = entity;
            entity->reference_count = 0;
            return entity;
        }
    
        virtual void destroy() override {
            TD_ASSERT(is_alive(), "Entity is not alive when destroyed");
            TD_ASSERT(is_allocated(), "Entity is not allocated");

            // We have special routine for this, because we do not want the components
            // remove themselves from the component chain

            flags &= ~internal::ComponentFlags::IsAlive;

            ComponentBase* current = this->next;
            while( current != this ) {
                TD_ASSERT(current->next != nullptr, "ComponentBase is not connected to another");
                ComponentBase* next = current->next;
                current->flags &= ~internal::ComponentFlags::IsAlive;

                if( current->reference_count == 0 ) {
                    current->free();
                }

                current = next;
            }

            if( reference_count == 0 ) {
                internal::Registry<Entity>::free_component(this); 
            }
        }

        template<typename TComponent>
        TComponent* add_component() {
            TD_ASSERT(is_alive(), "Entity is not alive");
            
            TComponent* component = internal::Registry<TComponent>::create_component();

            component->next = this;
            component->reference_count = 0;
            
            ComponentBase* current_last = get_last_component();
            current_last->next = component;

            return component;
        }

        uint16 get_num_components() {
            TD_ASSERT(is_alive(), "Entity is not alive");
            
            uint16 count = 0;

            ComponentMetaData* current = this->next;
            while( current != this ) {
                TD_ASSERT(current->next != nullptr, "ComponentMetaData is not connected to another");
                current = current->next;
                count++;
            }

            return count;
        }

    private:

        virtual void free() override final {
            internal::Registry<Entity>::free_component(static_cast<Entity*>(this));
        }

        Entity() = default;

        Entity(const Entity&) = delete;

        Entity(Entity&&) = delete;

        friend class internal::Registry<Entity>;
        friend class internal::RegistryBlock<Entity>;

    };
}