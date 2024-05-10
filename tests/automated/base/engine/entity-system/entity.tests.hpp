#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/entity-system/component.hpp>


namespace td::entity_tests {

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public: 

        using ConstructorStatistics::ConstructorStatistics;

        int32 a { 1 };
        int16 b { 2};
        int8 c { 3};
    };

    TD_TEST("entity-system/entity/basic-create") {
        TestComponent::reset_constructor_counters();

        Entity* entity = Entity::create();

        TestComponent* component_1 = entity->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, 1U);
        TD_TEST_ASSERT_EQUAL(TestComponent::num_default_constructors_called, 1U);
        TD_TEST_ASSERT_EQUAL(entity->get_num_components(), 1);
        TD_TEST_ASSERT_EQUAL(component_1->get_entity(), entity);
        TD_TEST_ASSERT_EQUAL(component_1->a, 1);
        TD_TEST_ASSERT_EQUAL(component_1->b, 2);
        TD_TEST_ASSERT_EQUAL(component_1->c, 3);

        TestComponent* component_2 = entity->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, 2U);
        TD_TEST_ASSERT_EQUAL(TestComponent::num_default_constructors_called, 2U);
        TD_TEST_ASSERT_EQUAL(entity->get_num_components(), 2);
        TD_TEST_ASSERT_EQUAL(component_2->get_entity(), entity);
        TD_TEST_ASSERT_EQUAL(component_2->a, 1);
        TD_TEST_ASSERT_EQUAL(component_2->b, 2);
        TD_TEST_ASSERT_EQUAL(component_2->c, 3);
    }

    TD_TEST("entity-system/entity/destroy") {
        TestComponent::reset_constructor_counters();

        Entity* entity = Entity::create();

        TestComponent* component_1 = entity->add_component<TestComponent>();
        TestComponent* component_2 = entity->add_component<TestComponent>();
        TestComponent* component_3 = entity->add_component<TestComponent>();

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);

        entity->destroy();

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 3U);

        TD_TEST_ASSERT_EQUAL(component_1->is_alive(), false);
        TD_TEST_ASSERT_EQUAL(component_2->is_alive(), false);
        TD_TEST_ASSERT_EQUAL(component_3->is_alive(), false);
    }

    TD_TEST("entity-system/entity/destroy-component") {
        TestComponent::reset_constructor_counters();

        Entity* entity = Entity::create();

        static_cast<void>(entity->add_component<TestComponent>());
        TestComponent* component_2 = entity->add_component<TestComponent>();
        static_cast<void>(entity->add_component<TestComponent>());

        component_2->destroy();

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 1U);
        TD_TEST_ASSERT_EQUAL(component_2->is_alive(), false);
        TD_TEST_ASSERT_EQUAL(entity->get_num_components(), 2);

        entity->destroy();

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called,3U);
    }

    // TD_TEST("entity-system/entity/test") {
        
    //     td::List<td::Entity*> entities;
    //     td::List<TestComponent*> components_to_delete;
    //     td::List<TestComponent*> components;
        
    //     for( td::uint32 i = 0; i < 25; i++ ) {
    //         td::Entity* e = td::Entity::create();
    //         entities.add(e);
    //         if( i % 2 == 0 ) {
    //             components.add(e->add_component<TestComponent>());
    //         }
    //         else {
    //             components_to_delete.add(e->add_component<TestComponent>());
    //         }
    //     }

    //     for( td::uint32 i = 0; i < components_to_delete.get_size(); i++ ) {
    //         components_to_delete[i]->destroy();
    //     }

    //     for( td::uint32 i = 0; i < components.get_size(); i++ ) {
    //         components[i]->destroy();
    //     }

    //     for(td::uint32 i = 0; i < entities.get_size(); i++ ) {
    //         entities[i]->destroy();
    //     }
    // }

}