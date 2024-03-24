#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/mat/mat3x3.dec.hpp>

namespace td::gte {

    // Overwrites the Rotation Matrix register
    [[nodiscard]] extern Mat3x3<Fixed16<12>> multiply(const Mat3x3<Fixed16<12>>& left, const Mat3x3<Fixed16<12>>& right);

    // Overwrites rotation matrix register and translation vector register
    [[nodiscard]] extern Vec3<Fixed32<12>> multiply(const Mat3x3<Fixed16<12>>& m, const Vec3<Fixed32<12>>& v);
    
    [[nodiscard]] extern Vec3<Fixed16<12>> normalize(const Vec3<Fixed32<12>>& vec);

    // Create a matrix that rotates a vector in order zyx (rotation vector is xyz, where 1 is 360 degrees)
    [[nodiscard]] extern Mat3x3<Fixed16<12>> rotation_matrix(const Vec3<Fixed16<12>>& rotations);

}