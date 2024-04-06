#pragma once

#include "tundra/engine/transform-matrix.hpp"
#include <psxgte.h>

#include <tundra/core/fixed.fwd.hpp>
#include <tundra/core/vec/vec.fwd.hpp>
#include <tundra/core/mat/mat3x3.fwd.hpp>

namespace td::gte {

    static MATRIX& to_gte_matrix_ref(const TransformMatrix& object) {
        return const_cast<MATRIX&>(reinterpret_cast<const MATRIX&>(object));
    }

    // WARNING: The translation of the output matrix is not valid
    static MATRIX& to_gte_matrix_ref(const Mat3x3<Fixed16<12>>& object) {
        return const_cast<MATRIX&>(reinterpret_cast<const MATRIX&>(object));
    }

    // TODO: Implement rest of these

}