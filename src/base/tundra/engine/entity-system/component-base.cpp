#include <tundra/engine/entity-system/internal/component-base.hpp>

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/entity.hpp>


namespace td::internal {

    [[nodiscard]] uint8 ComponentBase::get_reference_count() const {
        return reference_count;
    }

    [[nodiscard]] Entity* ComponentBase::get_entity() {
        TD_ASSERT(is_alive(), "ComponentBase must be alive");

        if( is_entity() ) return static_cast<Entity*>(this);

        ComponentBase* current = this->next;
        while( current != this ) {
            if( current->is_entity() ) return static_cast<Entity*>(current);
            current = current->next;
        }

        TD_ASSERT(false, "ComponentMetaData does not have an entity");
    }

    // Returns this if the next component is an Entity or if 
    [[nodiscard]] ComponentBase* ComponentBase::get_last_component() { 
        TD_ASSERT(is_alive(), "ComponentBase must be alive");

        ComponentBase* current = this;
        while( true ) {
            TD_ASSERT(current->next != nullptr, "ComponentMetaData is not connected to another");
            if( current->next->is_entity() ) return current;
            current = current->next;
        }

        return nullptr;
    }

    // Returns this if the next component is an Entity or if 
    [[nodiscard]] ComponentBase* ComponentBase::get_previous_component() { 
        TD_ASSERT(is_alive(), "ComponentBase must be alive");

        ComponentBase* current = this;
        while( true ) {
            TD_ASSERT(current->next != nullptr, "ComponentMetaData is not connected to another");
            if( current->next == this ) return current;
            current = current->next;
        }
    }

}