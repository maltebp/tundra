#pragma once

#include "tundra/core/fixed.hpp"
#include "tundra/core/vec/vec3.dec.hpp"
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/camera.fwd.hpp>

namespace td::gte {


    extern const TransformMatrix& compute_world_matrix(const Transform* transform);

    extern const TransformMatrix& compute_world_matrix(const DynamicTransform* transform);

    extern TransformMatrix compute_world_matrix(
        Vec3<Fixed32<12>> scale, 
        Vec3<Fixed16<12>> rotation, 
        Vec3<Fixed32<12>> translation, 
        const Transform* transform_parent = nullptr);

    // This is not cached
    extern TransformMatrix compute_world_matrix_inverse(const DynamicTransform* transform);

    // The transform is not cached
    extern TransformMatrix compute_camera_matrix(const Camera* camera);

    // Overwrites the rotation and translation matrix registers
    // Note: the fact that it takes a 16-bit vector but returns 32-bit is a limitation of GTE
    extern Vec3<Fixed32<12>> apply_transform_matrix(const TransformMatrix& m, const Vec3<Fixed16<12>>& v);

    extern TransformMatrix multiply_transform_matrices(const TransformMatrix& m1, const TransformMatrix& m2);

    // Extracts a matrix that describe only the rotation of the transform matrix,
    // by normalizing the first 3 column in the scale-rotation matrix.
    extern Mat3x3<Fixed16<12>> extract_rotation_matrix(const TransformMatrix& transform_matrix);

}