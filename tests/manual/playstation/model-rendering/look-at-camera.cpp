#include "look-at-camera.hpp"

#include <psxgpu.h>

#include "assert.h"

#include "globals.hpp"


const SVECTOR UP_DIRECTION = { 0, ONE, 0 };


VECTOR cross_product(const SVECTOR& v0, const SVECTOR& v1) {
    return {
        ((v0.vy * v1.vz) - (v0.vz * v1.vy)) >> 12,
        ((v0.vz * v1.vx) - (v0.vx * v1.vz)) >> 12,
        ((v0.vx * v1.vy) - (v0.vy * v1.vx)) >> 12
    };
}

void LookAtCamera::set_target(VECTOR target) {
    this->target = target;
}

void LookAtCamera::set_position(VECTOR position) {
    this->position = position;
}

MATRIX LookAtCamera::compute_matrix() const {

    // ASSERT(position != target, "Target and position must not be equal"); // Missing equality method of vector
    
	SVECTOR zaxis;
	SVECTOR xaxis;
	SVECTOR yaxis;

    VECTOR target_axis {
        target.vx - position.vx,
        target.vy - position.vy,
        target.vz - position.vz
    };

	VectorNormalS(&target_axis, &zaxis);

    target_axis = cross_product(UP_DIRECTION, zaxis);
	VectorNormalS(&target_axis, &xaxis);

	target_axis = cross_product(xaxis, zaxis);
	VectorNormalS(&target_axis, &yaxis);

    MATRIX world_to_view_matrix {{ 
            xaxis.vx, xaxis.vy, xaxis.vz,
            yaxis.vx, yaxis.vy, yaxis.vz,
            zaxis.vx, zaxis.vy, zaxis.vz
    }};

    VECTOR position_offset = { -position.vx >> MATH_SCALE, -position.vy >> MATH_SCALE, -position.vz >> MATH_SCALE };
    VECTOR translation;

	ApplyMatrixLV(&world_to_view_matrix, &position_offset, &translation);
    TransMatrix(&world_to_view_matrix, &translation);
    
    return world_to_view_matrix;
}

