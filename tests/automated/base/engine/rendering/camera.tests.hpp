#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/core/list.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/camera.hpp>

namespace td::camera_tests {

    Camera* create_test_camera() {
        Entity* entity = Entity::create();
        DynamicTransform* transform = entity->add_component<DynamicTransform>();
        List<CameraLayerSettings> layer_settings;
        layer_settings.add(CameraLayerSettings{0, 1});
        Camera* camera = entity->add_component<Camera>(*transform, layer_settings);
        return camera;
    }

    TD_TEST("camera/look-at") {
     
        Camera* camera = create_test_camera(); 
        camera->transform->set_translation(Vec3<Fixed32<12>>{0, 0, 0});
        camera->look_at();

    }

}