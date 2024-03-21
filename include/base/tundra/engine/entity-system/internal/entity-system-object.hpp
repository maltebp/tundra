#pragma once

#include <tundra/core/types.hpp>

namespace td {
    class Entity;
}

namespace td::internal {

    enum class EntitySystemObjectFlags : byte {
        IsAlive = 1 << 0
    };

    static inline EntitySystemObjectFlags operator|(EntitySystemObjectFlags a, EntitySystemObjectFlags b) {
        return static_cast<EntitySystemObjectFlags>(static_cast<byte>(a) | static_cast<byte>(b));
    }

    static inline EntitySystemObjectFlags& operator|=(EntitySystemObjectFlags& a, EntitySystemObjectFlags b) {
        a = a | b;
        return a;
    }

    class EntitySystemObject {
    public:

        [[nodiscard]] virtual bool is_entity() const = 0;

        [[nodiscard]] bool is_component() const { return !is_entity(); }

        [[nodiscard]] Entity* get_entity();

        [[nodiscard]] const Entity* get_entity() const;

    };

}