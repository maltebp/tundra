#include "tundra/gte/operations.hpp"

#include <psxgte.h>

#include <tundra/core/assert.hpp>
#include <tundra/gte/initialize.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/mat/mat3x3.hpp>


namespace td::gte {

    // Overwrites the Rotation Matrix register
    [[nodiscard]] Mat3x3<Fixed16<12>> multiply(const Mat3x3<Fixed16<12>>& left, const Mat3x3<Fixed16<12>>& right) {
        TD_ASSERT(is_initialized(), "GTE is not initialized");

        Mat3x3<Fixed16<12>> result;

        // I think this casting is slightly dangerous, because we cannot validate
        // whether it is a standard-layout type (I think it is though)

        MATRIX* psn00b_left = const_cast<MATRIX*>(reinterpret_cast<const MATRIX*>(&left));
        MATRIX* psn00b_right = const_cast<MATRIX*>(reinterpret_cast<const MATRIX*>(&right));
        MATRIX* psn00b_result = const_cast<MATRIX*>(reinterpret_cast<const MATRIX*>(&result));

        MulMatrix0(psn00b_left, psn00b_right, psn00b_result);
        
        return result;
    }

    // Overwrites the Rotation Matrix register
    // [[nodiscard]] Vec3<Fixed16<12>> multiply(const Mat3x3<Fixed16<12>>& left, const Vec3<Fixed16<12>>& right) { }

    [[nodiscard]] Vec3<Fixed16<12>> normalize(const Vec3<Fixed32<12>>& vec) {
        TD_ASSERT(is_initialized(), "GTE is not initialized");
        
        Vec3<Fixed16<12>> result;
        
        VECTOR* psn00b_vec = const_cast<VECTOR*>(reinterpret_cast<const VECTOR*>(&vec));
        SVECTOR* psn00b_result = const_cast<SVECTOR*>(reinterpret_cast<const SVECTOR*>(&result));

        VectorNormalS(psn00b_vec, psn00b_result);

        return result;
    }

}