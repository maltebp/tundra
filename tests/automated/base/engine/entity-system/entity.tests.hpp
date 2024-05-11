#pragma once

#include "tundra/core/log.hpp"
#include "tundra/engine/entity-system/internal/registry.dec.hpp"
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

        entity->destroy();
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

        TD_TEST_ASSERT_EQUAL(td::internal::Registry<TestComponent>::get_num_allocated_components(), 0U);
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

    TD_TEST("entity-system/entity/get-all") {

        TD_ASSERT(
            internal::Registry<td::Entity>::get_num_allocated_components() == 0, 
            "Entities was not cleared before running the test"
        );

        td::List<Entity*> not_found_entities;
        td::List<Entity*> not_destroyed_entities;

        for( td::uint32 i = 0; i < 100; i++ ) {
            Entity* entity = Entity::create();    
            not_found_entities.add(entity);
            not_destroyed_entities.add(entity);
        }

        for( td::Entity* entity : td::Entity::get_all() ) {
            bool entity_was_found = not_found_entities.remove(entity);
            TD_TEST_ASSERT_EQUAL(entity_was_found, true);           
        }

        TD_TEST_ASSERT_EQUAL(not_found_entities.get_size(), 0U);           

        // Destroying the entities
        for( td::Entity* entity : td::Entity::get_all() ) {
            entity->destroy();
            
            bool was_found = not_destroyed_entities.remove(entity);
            TD_TEST_ASSERT_EQUAL(was_found, true);                       
        }
        TD_TEST_ASSERT_EQUAL(not_destroyed_entities.get_size(), 0U);       
        
        // Ensuring there are no more entities
        td::uint32 num_entities_remaining = 0U;
        for( [[maybe_unused]] td::Entity* entity : td::Entity::get_all() ) {
            num_entities_remaining++;
        }

        TD_TEST_ASSERT_EQUAL(num_entities_remaining, 0U);       
    }

}