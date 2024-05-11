#pragma once

#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

namespace td::internal {

    // Created this class, because I realized late that I cannot avoid having
    // virtual functions, and the original base class (ComponentMetaData), cannot
    // be virtual, because I need to be able to do downcasts using static_cast.
    // In the future, this could maybe be refactored.

    class ComponentBase : public ComponentMetaData {
    public:

        // Mark this component/entity as destroyed - this deallocate and call
        // destructor once all references to the it has been released
        virtual void destroy() = 0;

        virtual void free() = 0;

        virtual void on_destroy() { }

        [[nodiscard]] uint8 get_reference_count() const;

        [[nodiscard]] Entity* get_entity();

        // Returns this if the next component is an Entity
        [[nodiscard]] ComponentBase* get_last_component();

        // Note this might be an Entity
        [[nodiscard]] ComponentBase* get_previous_component();

    protected:

        virtual ~ComponentBase() = default;

    };

}