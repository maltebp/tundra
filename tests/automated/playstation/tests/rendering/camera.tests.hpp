#pragma once

#include "tundra/core/vec/vec3.dec.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/core/list.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/camera.hpp>

#include <tundra/gte/compute-transform.hpp>

namespace td::camera_tests {

    static Camera* create_test_camera() {
        Entity* entity = Entity::create();
        DynamicTransform* transform = entity->add_component<DynamicTransform>();
        List<CameraLayerSettings> layer_settings;
        layer_settings.add(CameraLayerSettings{0, 1});
        Camera* camera = entity->add_component<Camera>(transform, layer_settings);
        return camera;
    }

    TD_TEST("camera/look-at") {
        
        constexpr td::Fixed32<12> ALLOWED_ERROR = to_fixed(0.0078125);

        Camera* camera = create_test_camera(); 

        Vec3<td::Fixed32<12>> some_position { 1, 1, 1 };

        // No change
        {
            Vec3<td::Fixed32<12>> translation { 0 };
            Vec3<td::Fixed32<12>> look_at_target { 0, 0, 1}; 
            Vec3<td::Fixed32<12>> expected = some_position;

            camera->transform->set_translation(translation);
            camera->look_at(look_at_target);
            const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
            Vec3<td::Fixed32<12>> result_difference = gte::apply_transform_matrix(camera_matrix, some_position) - expected; 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -Vec3<Fixed32<12>>{ ALLOWED_ERROR });
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, Vec3<Fixed32<12>>{ ALLOWED_ERROR });
        }

        // Looking from positive z-axis
        {
            Vec3<td::Fixed32<12>> translation { 0, 0, 2 };
            Vec3<td::Fixed32<12>> look_at_target { 0, 0, 0}; 
            Vec3<td::Fixed32<12>> expected = { -1, 1, 1};

            camera->transform->set_translation(translation);
            camera->look_at(look_at_target);
            const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
            Vec3<td::Fixed32<12>> result_difference = gte::apply_transform_matrix(camera_matrix, some_position) - expected; 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -Vec3<Fixed32<12>>{ ALLOWED_ERROR });
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, Vec3<Fixed32<12>>{ ALLOWED_ERROR });
        }

        // Looking from positive x-axis
        {
            Vec3<td::Fixed32<12>> translation { 2, 0, 0 };
            Vec3<td::Fixed32<12>> look_at_target { 0, 0, 0}; 
            Vec3<td::Fixed32<12>> expected = { 1, 1, 1};

            camera->transform->set_translation(translation);
            camera->look_at(look_at_target);
            const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
            Vec3<td::Fixed32<12>> result_difference = gte::apply_transform_matrix(camera_matrix, some_position) - expected; 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -Vec3<Fixed32<12>>{ ALLOWED_ERROR });
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, Vec3<Fixed32<12>>{ ALLOWED_ERROR });
        }

        // Tilting up
        {
            Vec3<td::Fixed32<12>> translation {1, 0, 0 };
            Vec3<td::Fixed32<12>> look_at_target { some_position }; 
            Vec3<td::Fixed32<12>> expected = { 0, 0, to_fixed(1.41421356237)};

            camera->transform->set_translation(translation);
            camera->look_at(look_at_target);
            const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
            Vec3<td::Fixed32<12>> result_difference = gte::apply_transform_matrix(camera_matrix, some_position) - expected; 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -Vec3<Fixed32<12>>{ ALLOWED_ERROR });
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, Vec3<Fixed32<12>>{ ALLOWED_ERROR });
        }
    }

}