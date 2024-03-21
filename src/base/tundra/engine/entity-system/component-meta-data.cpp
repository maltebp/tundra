#include <tundra/engine/entity-system/internal/component-meta-data.hpp>

#include <tundra/core/assert.hpp>

#include <tundra/engine/entity-system/entity.hpp>


namespace td::internal {

    [[nodiscard]] bool ComponentMetaData::is_alive() const { 
        return (flags & ComponentFlags::IsAlive) == ComponentFlags::IsAlive;
    }

    [[nodiscard]] bool ComponentMetaData::is_allocated() const { 
        return (flags & ComponentFlags::IsAllocated) == ComponentFlags::IsAllocated;
    }

    [[nodiscard]] bool ComponentMetaData::is_entity() const { 
        return (flags & ComponentFlags::IsEntity) == ComponentFlags::IsEntity;
    }

}