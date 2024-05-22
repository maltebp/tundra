#pragma once

#include <tundra/engine/entity-system/component.hpp>

class SmallComponent : public td::Component<SmallComponent> {
public:
    volatile td::byte data[1];
};

class MediumComponent : public td::Component<MediumComponent> {
public:
    volatile td::byte data[16];
};

class LargeComponent : public td::Component<LargeComponent> {
public:
    volatile td::byte data[64];
};

namespace internal {
    static volatile td::uint32 count = 0;
}

template<typename TComponent>
void iterate_components() {
    internal::count = 0;

    for( [[maybe_unused]] TComponent* component : TComponent::get_all() ) {
        component->data[0] = component->data[0] + 1;
    }
}