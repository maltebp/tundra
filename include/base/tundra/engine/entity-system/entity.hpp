#pragma once

#include "tundra/core/utility.hpp"
#include <tundra/engine/entity-system/internal/component-base.hpp>
#include <tundra/engine/entity-system/internal/component-flags.hpp>
#include <tundra/engine/entity-system/internal/registry.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>
#include <tundra/engine/entity-system/internal/registry-block.hpp>
#include <type_traits>

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
            internal::ComponentBase::destroy_group();
        }

        template<typename TComponent, typename ... TArgs>
        TComponent* add_component(TArgs&& ... args) {
            static_assert(
                std::is_base_of<internal::ComponentBase, TComponent>::value,
                "TComponent must derive from td::Component<TComponent, TOptionalBase>");

            TD_ASSERT(is_alive(), "Entity is not alive");
            
            TComponent* component = internal::Registry<TComponent>::create_component(forward<TArgs>(args)...);

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

        static internal::Registry<Entity>::Iterable get_all() {
            return internal::Registry<Entity>::get_all();
        }

    private:

        virtual void free() override final {
            internal::Registry<Entity>::free_component(this);
        }

        Entity() = default;

        Entity(const Entity&) = delete;

        Entity(Entity&&) = delete;

        friend class internal::Registry<Entity>;
        friend class internal::RegistryBlock<Entity>;

    };
}