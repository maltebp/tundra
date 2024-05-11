#pragma once

#include "tundra/engine/entity-system/internal/registry-block.dec.hpp"
#include "tundra/engine/entity-system/internal/registry.dec.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/dynamic-transform.hpp>

namespace td::dynamic_transform_tests {

    TD_TEST("engine/dynamic-transform/no-children") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();
        
        DynamicTransform* transform = e->add_component<DynamicTransform>();
        TD_TEST_ASSERT_EQUAL(transform->get_num_children(), 0U);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/one-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 1U);

        TD_TEST_ASSERT_EQUAL(child_1->get_parent(), parent);
        TD_TEST_ASSERT_EQUAL(child_1->get_num_children(), 0U);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/multiple-children") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 1U);
        TD_TEST_ASSERT_EQUAL(child_1->get_parent(), parent);
        TD_TEST_ASSERT_EQUAL(child_1->get_num_children(), 0U);
        
        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 2U);
        TD_TEST_ASSERT_EQUAL(child_2->get_parent(), parent);
        TD_TEST_ASSERT_EQUAL(child_2->get_num_children(), 0U);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 3U);
        TD_TEST_ASSERT_EQUAL(child_3->get_parent(), parent);
        TD_TEST_ASSERT_EQUAL(child_3->get_num_children(), 0U);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/removing-only-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child = e->add_component<DynamicTransform>();
        parent->add_child(child);
        parent->remove_child(child);

        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 0U);
        TD_TEST_ASSERT_EQUAL(child->get_parent(), nullptr);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/removing-first-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);
        
        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);

        parent->remove_child(child_1);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 2U);
        TD_TEST_ASSERT_EQUAL(child_1->get_parent(), nullptr);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/removing-middle-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);
        
        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);

        parent->remove_child(child_2);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 2U);
        TD_TEST_ASSERT_EQUAL(child_2->get_parent(), nullptr);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/removing-last-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);
        
        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);

        parent->remove_child(child_3);
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 2U);
        TD_TEST_ASSERT_EQUAL(child_3 ->get_parent(), nullptr);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/destroying-only-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);

        child_1->destroy();
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 0U);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/destroying-middle-child") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);

        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);

        child_2->destroy();
        TD_TEST_ASSERT_EQUAL(parent->get_num_children(), 2U);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/destroying-parent") {
                TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        Entity* e = Entity::create();

        DynamicTransform* parent = e->add_component<DynamicTransform>();

        DynamicTransform* child_1 = e->add_component<DynamicTransform>();
        parent->add_child(child_1);

        DynamicTransform* child_2 = e->add_component<DynamicTransform>();
        parent->add_child(child_2);

        DynamicTransform* child_3 = e->add_component<DynamicTransform>();
        parent->add_child(child_3);

        parent->destroy();

        TD_TEST_ASSERT_EQUAL(child_1->get_parent(), nullptr);
        TD_TEST_ASSERT_EQUAL(child_2->get_parent(), nullptr);
        TD_TEST_ASSERT_EQUAL(child_3->get_parent(), nullptr);

        e->destroy();
    }

    TD_TEST("engine/dynamic-transform/destroying-entities-destroys-transforms") {

        TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);

        auto add_entity_transform = [](td::DynamicTransform* parent) {
            td::Entity* entity = td::Entity::create();
            td::DynamicTransform* transform = entity->add_component<td::DynamicTransform>();
            if( parent != nullptr ) parent->add_child(transform);
            return transform;
        };

        td::DynamicTransform* root = add_entity_transform(nullptr);

        td::DynamicTransform* child_1 = add_entity_transform(root);
        add_entity_transform(child_1);
        add_entity_transform(child_1);
        add_entity_transform(child_1);

        td::DynamicTransform* child_2 = add_entity_transform(root);
        add_entity_transform(child_2);
        add_entity_transform(child_2);
        add_entity_transform(child_2);

        td::DynamicTransform* child_3 = add_entity_transform(root);
        add_entity_transform(child_3);
        add_entity_transform(child_3);
        add_entity_transform(child_3);

        TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 13U);
        
        for( td::Entity* entity : td::Entity::get_all() ) {
            entity->destroy();
        }

        TD_TEST_ASSERT_EQUAL(internal::Registry<DynamicTransform>::get_num_allocated_components(), 0U);
    }

}