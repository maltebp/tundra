#include "tundra/core/assert.hpp"
#include "tundra/core/fixed.hpp"
#include "tundra/engine/static-transform.hpp"
#include <tundra/gte/compute-transform.hpp>

#include <psxgte.h>

#include <tundra/gte/initialize.hpp>
#include <tundra/gte/operations.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/transform-matrix.hpp>
#include <tundra/engine/transform.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/core/mat/mat3x3.hpp>
#include <tundra/engine/entity-system/entity.hpp>

namespace td::gte { 

    namespace internal {
        Mat3x3<Fixed16<12>> compute_scale_rotation(
            Vec3<Fixed32<12>> scale, 
            Vec3<Fixed16<12>> rotation, 
            const TransformMatrix* parent_matrix = nullptr);

        Vec3<Fixed32<12>> compute_translation(
            Vec3<Fixed32<12>> translate, 
            const TransformMatrix* parent_matrix = nullptr);
    }

    extern const TransformMatrix& compute_world_matrix(const Transform* transform) {        
        switch(transform->get_type()) {
            case td::TransformType::Static:
                return static_cast<const StaticTransform*>(transform)->world_matrix;
            case td::TransformType::Dynamic:
                return compute_world_matrix(static_cast<const DynamicTransform*>(transform));
            default:
                TD_ASSERT(false, "Unknown TransformType");
        }
    }

    extern const TransformMatrix& compute_world_matrix(const DynamicTransform* transform) {
        TD_ASSERT(gte::is_initialized(), "GTE is not initialized");

        const TransformMatrix* parent_matrix = transform->parent == nullptr ? nullptr : &compute_world_matrix(transform->parent);

        bool scale_or_rotation_are_dirty = (transform->dirty_flags & DynamicTransform::DirtyFlags::RotationAndScale) == DynamicTransform::DirtyFlags::RotationAndScale;
        if( scale_or_rotation_are_dirty ) {
            transform->cached_world_matrix.scale_and_rotation = internal::compute_scale_rotation(
                transform->scale, transform->rotation, parent_matrix);
        }

        bool translation_is_dirty = (transform->dirty_flags & DynamicTransform::DirtyFlags::Translation) == DynamicTransform::DirtyFlags::Translation;
        if( translation_is_dirty ) {
            transform->cached_world_matrix.translation = internal::compute_translation(
                transform->translation, parent_matrix);
        }

        transform->dirty_flags = DynamicTransform::DirtyFlags::None;

        return transform->cached_world_matrix;

        // TODO: I believe this can be heavily optimized:
        //  - No reason to rotate an axis, if angle is 0 (right now we rotate all)
        //  - We can probably avoid some "matrix register uploads" (e.g. store intermediate in result matrix)
    }

    extern TransformMatrix compute_world_matrix(
        Vec3<Fixed32<12>> scale, 
        Vec3<Fixed16<12>> rotation, 
        Vec3<Fixed32<12>> translation, 
        const Transform* transform_parent
    ) {
        const TransformMatrix* parent_matrix = 
            transform_parent == nullptr ? nullptr : &compute_world_matrix(transform_parent);

        return TransformMatrix{
            internal::compute_scale_rotation(scale, rotation, parent_matrix),
            internal::compute_translation(translation, parent_matrix)    
        };
    }

    Vec3<Fixed32<12>> apply_transform_matrix(const TransformMatrix& m, const Vec3<Fixed32<12>>& v) {
        TD_ASSERT(gte::is_initialized(), "GTE is not initialized");

        MATRIX* raw_matrix = const_cast<MATRIX*>(reinterpret_cast<const MATRIX*>(&m));
        VECTOR* raw_vector = const_cast<VECTOR*>(reinterpret_cast<const VECTOR*>(&v));
        
        Vec3<Fixed32<12>> result;
        VECTOR* raw_result = const_cast<VECTOR*>(reinterpret_cast<const VECTOR*>(&result));
        
        ApplyMatrixLV(raw_matrix, raw_vector, raw_result);

        result += m.translation;
        
        return result;
    }

    
    Vec3<Fixed32<12>> internal::compute_translation(
            Vec3<Fixed32<12>> translation, 
            const TransformMatrix* parent_matrix) {

        if( parent_matrix == nullptr ) {
            return translation;
        }
        else {
            return parent_matrix->translation + gte::multiply(parent_matrix->scale_and_rotation, translation);
        }
    }

    extern Mat3x3<Fixed16<12>> internal::compute_scale_rotation(
            Vec3<Fixed32<12>> scale, 
            Vec3<Fixed16<12>> rotation, 
            const TransformMatrix* parent_matrix) {
            
        if( parent_matrix == nullptr ) {
            bool rotation_is_zero =  rotation == Vec3<Fixed16<12>>{0};   
            if( rotation_is_zero ) {
                // We don't care if scale is 1 - we have to initialize the matrix to
                // identity anyway
                return {
                    (Fixed16<12>)scale.x, 0, 0,
                    0, (Fixed16<12>)scale.y, 0,
                    0, 0, (Fixed16<12>)scale.z
                };
            }
            else {
                Mat3x3<Fixed16<12>> rotation_matrix = gte::rotation_matrix(rotation);
                if( scale == Vec3<Fixed32<12>>{1} ) {
                    return rotation_matrix;
                }
                else {
                    Mat3x3<Fixed16<12>> scale_matrix {
                        (Fixed16<12>)scale.x, 0, 0,
                        0, (Fixed16<12>)scale.y, 0,
                        0, 0, (Fixed16<12>)scale.z
                    };
                    
                    return gte::multiply(rotation_matrix, scale_matrix);
                }
            }
        }
        else {
            bool scale_is_one =  scale == Vec3<Fixed32<12>>{1};   
            bool rotation_is_zero =  rotation == Vec3<Fixed16<12>>{0}; 
            
            if( rotation_is_zero && scale_is_one ) {
                return parent_matrix->scale_and_rotation;
            }
            else if( rotation_is_zero ) {
                Mat3x3<Fixed16<12>> scale_matrix {
                    (Fixed16<12>)scale.x, 0, 0,
                    0, (Fixed16<12>)scale.y, 0,
                    0, 0, (Fixed16<12>)scale.z
                };
                return gte::multiply(parent_matrix->scale_and_rotation, scale_matrix);
            }
            else if( scale_is_one ) {
                td::Mat3x3 rotation_matrix = gte::rotation_matrix(rotation);
                return gte::multiply(parent_matrix->scale_and_rotation, rotation_matrix);
            }
            else {
                // Both scale and rotation are set
                Mat3x3<Fixed16<12>> scale_matrix {
                    (Fixed16<12>)scale.x, 0, 0,
                    0, (Fixed16<12>)scale.y, 0,
                    0, 0, (Fixed16<12>)scale.z
                };
                td::Mat3x3 rotation_matrix = gte::rotation_matrix(rotation);
                td::Mat3x3 scale_and_rotation = gte::multiply(rotation_matrix, scale_matrix);
                return multiply(parent_matrix->scale_and_rotation, scale_and_rotation);
            }
        }
    }

}