#pragma once

#include "tundra/core/fixed.hpp"
#include "tundra/core/vec/vec3.dec.hpp"
#include <tundra/engine/dynamic-transform.hpp>

namespace td::gte {


    extern const TransformMatrix& compute_world_matrix(const Transform* transform);

    extern const TransformMatrix& compute_world_matrix(const DynamicTransform* transform);

    extern TransformMatrix compute_world_matrix(
        Vec3<Fixed32<12>> scale, 
        Vec3<Fixed16<12>> rotation, 
        Vec3<Fixed32<12>> translation, 
        const Transform* transform_parent = nullptr);

    // Overwrites the rotation and translation matrix registers
    extern Vec3<Fixed32<12>> apply_transform_matrix(const TransformMatrix& m, const Vec3<Fixed32<12>>& v);

}