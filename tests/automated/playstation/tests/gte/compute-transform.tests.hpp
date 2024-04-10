#pragma once

#include "tundra/core/fixed.hpp"
#include "tundra/core/vec/vec3.dec.hpp"
#include "tundra/engine/static-transform.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/vec/vec3.hpp>
#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/gte/compute-transform.hpp>
#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/rendering/camera.hpp>

namespace td::compute_transform_tests {

    TD_TEST("gte/compute-transform/default") {

        Entity* e = Entity::create();

        DynamicTransform* t = e->add_component<DynamicTransform>();

        TransformMatrix world_matrix = gte::compute_world_matrix(t);        

        TD_TEST_ASSERT_EQUAL(world_matrix.scale_and_rotation, Mat3x3<Fixed16<12>>::get_identity());

        Vec3<Fixed32<12>> expected_translation{ 0 };
        TD_TEST_ASSERT_EQUAL(world_matrix.translation, expected_translation);

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, v);

        e->destroy();
    }

    TD_TEST("gte/compute-transform/rotate-only") {
        
        Entity* e = Entity::create();

        DynamicTransform* t = e->add_component<DynamicTransform>();
        t->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});

        TransformMatrix world_matrix = gte::compute_world_matrix(t);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        Vec3<Fixed32<12>> expected { -2, 3, -1 };        
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);  

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/scale-only") {
        
        Entity* e = Entity::create();

        DynamicTransform* t = e->add_component<DynamicTransform>();
        t->set_scale({3, 2, td::to_fixed(0.5)});

        TransformMatrix world_matrix = gte::compute_world_matrix(t);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        Vec3<Fixed32<12>> expected { 3, 4, td::to_fixed(1.5) };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);  

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/translation-only") {
        
        Entity* e = Entity::create();

        DynamicTransform* t = e->add_component<DynamicTransform>();
        t->set_translation({-2, 1, 3});

        TransformMatrix world_matrix = gte::compute_world_matrix(t);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        Vec3<Fixed32<12>> expected { -1, 3, 6 };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);  

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/all") {
        
        Entity* e = Entity::create();

        DynamicTransform* t = e->add_component<DynamicTransform>();
        t->set_scale({3, 2, td::to_fixed(0.5)});
        t->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        t->set_translation({-2, 1, 3});

        TransformMatrix world_matrix = gte::compute_world_matrix(t);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        
        // Scale: 3, 4, 1.5
        // Rotate: -4, 1.5, -3
        // Translation: -6, 2.5, 0
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.5), td::to_fixed(0) };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);  

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/with-parent/both-default") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        DynamicTransform* child = e->add_component<DynamicTransform>();
        parent->add_child(child);

        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, v); 

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/with-parent/parent-all-child-default") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        parent->set_scale({3, 2, td::to_fixed(0.5)});
        parent->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        parent->set_translation({-2, 1, 3});

        DynamicTransform* child = e->add_component<DynamicTransform>();
        parent->add_child(child);

        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.5), td::to_fixed(0) };

        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected); 

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/with-parent/parent-default-child-all") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        
        DynamicTransform* child = e->add_component<DynamicTransform>();
        child->set_scale({3, 2, td::to_fixed(0.5)});
        child->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        child->set_translation({-2, 1, 3});
        parent->add_child(child);

        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.5), td::to_fixed(0) };

        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected); 

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/with-parent/all") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        parent->set_scale({td::to_fixed(0.25), td::to_fixed(1), td::to_fixed(0.5)});
        parent->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        parent->set_translation({-2, 1, 3});

        DynamicTransform* child = e->add_component<DynamicTransform>();
        child->set_scale({td::to_fixed(0.5), td::to_fixed(1.5), 1});
        child->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        child->set_translation({-2, 1, 3});

        parent->add_child(child);

        Vec3<Fixed32<12>> v { 1, 2, 3 };

        // Child's transformation: 
        //  Scale: 0.5, 3, 3
        //  Rotate: -3, 3, -0.5
        //  Translation: -5, 4, 2.5
        // Parent's transformation: 
        //  Scale: -1.250, 4, 1.25
        //  Rotate: -4, 1.25, 1.25
        //  Translation: -6, 2.25, 4.25
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.25), td::to_fixed(4.25) };

        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected); 

        e->destroy(); 
    }

    TD_TEST("gte/compute-transform/with-parent/dirty-flags-correctly-set") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        DynamicTransform* child = e->add_component<DynamicTransform>();
        parent->add_child(child);

        Vec3<Fixed32<12>> v { 1, 1, 1 };
        Vec3<Fixed32<12>> result;
        Vec3<Fixed32<12>> expected;
        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        // Child rotation changes
        child->set_rotation({td::to_fixed(0.5), 0, 0});
        expected = { 1, -1, -1 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Child scale changes
        child->set_scale({1, td::to_fixed(0.5), 1});
        expected = { 1, td::to_fixed(-0.5), -1 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Child translation changes
        child->set_translation({1, 2, -1});
        expected = { 2, td::to_fixed(1.5), -2 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Parent rotation changes
        parent->set_rotation({0, 0, td::to_fixed(0.25)});
        expected = { td::to_fixed(-1.5), 2, -2 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Parent scale changes
        parent->set_scale({td::to_fixed(0.5), td::to_fixed(0.5), td::to_fixed(0.5)});
        expected = { td::to_fixed(-0.75), 1, -1 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Parent translation changes
        parent->set_translation({1, 1, 1});
        expected = { td::to_fixed(0.25), 2, 0 };
        world_matrix = gte::compute_world_matrix(child);     
        result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);

        e->destroy(); 
    }

    TD_TEST("gte/compute_matrix/static_transform/no-parent") {
        
        Entity* e = Entity::create();
        StaticTransform* transform = e->add_component<StaticTransform>(
            gte::compute_world_matrix(
                {3, 2, td::to_fixed(0.5)},
                {td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)},
                {-2, 1, 3}
            )
        );        

        TransformMatrix world_matrix = gte::compute_world_matrix(transform);     

        Vec3<Fixed32<12>> v { 1, 2, 3 };
        
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.5), td::to_fixed(0) };
        
        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected);  

        e->destroy();
    }

    TD_TEST("gte/compute_matrix/static_transform/parent") {
        
        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();
        parent->set_scale({td::to_fixed(0.25), td::to_fixed(1), td::to_fixed(0.5)});
        parent->set_rotation({td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)});
        parent->set_translation({-2, 1, 3});

        StaticTransform* child = e->add_component<StaticTransform>(gte::compute_world_matrix(
            {td::to_fixed(0.5), td::to_fixed(1.5), 1},
            {td::to_fixed(0.25), td::to_fixed(0.5), td::to_fixed(0.75)},
            {-2, 1, 3},
            parent
        ));

        Vec3<Fixed32<12>> v { 1, 2, 3 };

        // Child's transformation: 
        //  Scale: 0.5, 3, 3
        //  Rotate: -3, 3, -0.5
        //  Translation: -5, 4, 2.5
        // Parent's transformation: 
        //  Scale: -1.250, 4, 1.25
        //  Rotate: -4, 1.25, 1.25
        //  Translation: -6, 2.25, 4.25
        Vec3<Fixed32<12>> expected { -6, td::to_fixed(2.25), td::to_fixed(4.25) };

        TransformMatrix world_matrix = gte::compute_world_matrix(child);     

        Vec3<Fixed32<12>> result = gte::apply_transform_matrix(world_matrix, v);
        TD_TEST_ASSERT_EQUAL(result, expected); 

        e->destroy();
    }

    static Camera* create_test_camera() {
        Entity* entity = Entity::create();
        DynamicTransform* transform = entity->add_component<DynamicTransform>();
        List<CameraLayerSettings> layer_settings;
        layer_settings.add(CameraLayerSettings{0, 1});
        Camera* camera = entity->add_component<Camera>(transform, layer_settings);
        return camera;
    }

    TD_TEST("gte/compute_transform/compute_camera_matrix/identity") {
     
        Camera* camera = create_test_camera(); 

        Vec3<td::Fixed32<12>> some_position { 1, 1, 1 };

        const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
        Vec3<td::Fixed32<12>> transformed_position = gte::apply_transform_matrix(camera_matrix, some_position);
        Vec3<td::Fixed32<12>> expected { some_position.x, -some_position.y, some_position.z }; 
        TD_TEST_ASSERT_EQUAL(transformed_position, expected);
    }

    TD_TEST("gte/compute_transform/compute_camera_matrix/translation") {
     
        Camera* camera = create_test_camera();
        Vec3<td::Fixed32<12>> some_position { 1, 1, 1 };

        camera->transform->set_translation(Vec3<Fixed32<12>>{2, -1, -2}); 

        const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
        Vec3<td::Fixed32<12>> transformed_position = gte::apply_transform_matrix(camera_matrix, some_position);
        Vec3<td::Fixed32<12>> expected { -1, -2, 3 };
        TD_TEST_ASSERT_EQUAL(transformed_position, expected);
    }

    TD_TEST("gte/compute_transform/compute_camera_matrix/rotation") {
     
        Camera* camera = create_test_camera();
        Vec3<td::Fixed32<12>> some_position { 1, 1, 1 };

        camera->transform->set_rotation(Vec3<Fixed16<12>>{0, td::to_fixed(0.5), 0}); 

        const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
        Vec3<td::Fixed32<12>> transformed_position = gte::apply_transform_matrix(camera_matrix, some_position);
        Vec3<td::Fixed32<12>> expected { -1, -1, -1 };
        TD_TEST_ASSERT_EQUAL(transformed_position, expected);
    }

    TD_TEST("gte/compute_transform/compute_camera_matrix/rotation_and_translation") {
     
        Camera* camera = create_test_camera();
        Vec3<td::Fixed32<12>> some_position { 1, 1, 1 };

        camera->transform->set_translation(Vec3<Fixed32<12>>{0, 1, 2});
        camera->transform->set_rotation(Vec3<Fixed16<12>>{0, to_fixed(0.5), 0 }); 

        const TransformMatrix& camera_matrix = gte::compute_camera_matrix(camera);
        Vec3<td::Fixed32<12>> transformed_position = gte::apply_transform_matrix(camera_matrix, some_position);
        Vec3<td::Fixed32<12>> expected { -1, 0, 1};
        TD_TEST_ASSERT_EQUAL(transformed_position, expected);
    }

}