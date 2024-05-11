#pragma once

#include <tundra/engine/entity-system/component.hpp>

class SmallComponent : public td::Component<SmallComponent> {
public:
    td::byte data;
};

class MediumComponent : public td::Component<MediumComponent> {
public:
    td::uint32 data1;
    td::uint32 data2;
    td::uint32 data3;
    td::uint32 data4;
};

class LargeComponent : public td::Component<LargeComponent> {
public:
    td::uint32 data1;
    td::uint32 data2;
    td::uint32 data3;
    td::uint32 data4;
    td::uint32 data5;
    td::uint32 data6;
    td::uint32 data7;
    td::uint32 data8;
    td::uint32 data9;
    td::uint32 data10;
    td::uint32 data11;
    td::uint32 data12;
    td::uint32 data13;
    td::uint32 data14;
    td::uint32 data15;
    td::uint32 data16;
};

namespace internal {
    static volatile td::uint32 count = 0;
}

template<typename TComponent>
void iterate_components() {
    internal::count = 0;

    for( [[maybe_unused]] TComponent* component : TComponent::get_all() ) {
        internal::count = internal::count + 1;
    }
}