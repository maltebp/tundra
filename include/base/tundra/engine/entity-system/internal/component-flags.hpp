#pragma once

#include <tundra/core/types.hpp>

namespace td::internal {
    
    // TODO: The lifecycle state probably shouldn't be flags

    enum class ComponentFlags : byte {
        None = 0,
        IsEntity = 1 << 0,
        IsAllocated = 1 << 1,
        IsAlive = 1 << 2,
        All = 0xFF
    };

    static inline ComponentFlags operator~(ComponentFlags flags) {
        return static_cast<ComponentFlags>(~static_cast<byte>(flags));
    }

    static inline ComponentFlags operator&(ComponentFlags a, ComponentFlags b) {
        return static_cast<ComponentFlags>(static_cast<byte>(a) & static_cast<byte>(b));
    }

    static inline ComponentFlags& operator&=(ComponentFlags& a, ComponentFlags b) {
        a = a & b;
        return a;
    }

    static inline ComponentFlags operator|(ComponentFlags a, ComponentFlags b) {
        return static_cast<ComponentFlags>(static_cast<byte>(a) | static_cast<byte>(b));
    }

    static inline ComponentFlags& operator|=(ComponentFlags& a, ComponentFlags b) {
        a = a | b;
        return a;
    }
}