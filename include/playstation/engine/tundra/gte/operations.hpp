#pragma once

#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/mat/mat3x3.dec.hpp>

namespace td::gte {

    // Overwrites the Rotation Matrix register
    [[nodiscard]] extern Mat3x3<Fixed16<12>> multiply(const Mat3x3<Fixed16<12>>& left, const Mat3x3<Fixed16<12>>& right);

    [[nodiscard]] extern Vec3<Fixed16<12>> normalize(const Vec3<Fixed32<12>>& vec);
    
    // TODO: I don't think there is a psn00bsdk operation to multiply a mat3x3 by a vector (only a matrix + translation ApplyCompLV)
    // Overwrites the Rotation Matrix register
    // [[nodiscard]] extern Vec3<Fixed16<12>> multiply(const Mat3x3<Fixed16<12>>& left, const Vec3<Fixed16<12>>& right);

}