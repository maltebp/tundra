#pragma once

#include <tundra/core/mat/mat3x3.dec.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/core/fixed.hpp>

namespace td {

    class TransformMatrix {
    public:
        Mat3x3<Fixed16<12>> scale_and_rotation;
        Vec3<Fixed32<12>> translation;
    };
}