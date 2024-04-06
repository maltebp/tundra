#pragma once

#include "tundra/core/assert.hpp"
#include "tundra/engine/entity-system/component-ref.hpp"
#include "tundra/engine/entity-system/internal/registry.dec.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/entity-system/component.hpp>

namespace td::entity_system_chaotic_test {

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public: 

        using ConstructorStatistics::ConstructorStatistics;

        int32 a { 1 };
        int16 b { 2};
        int8 c { 3};
    };

    static Entity* create_test_entity(List<ComponentRef<TestComponent>*>& component_refs) {
        Entity* entity = Entity::create();
        component_refs.add(new ComponentRef<TestComponent>(entity->add_component<TestComponent>()));
        component_refs.add(new ComponentRef<TestComponent>(entity->add_component<TestComponent>()));
        return entity;
    }

    static List<Entity*> create_entity_set(uint32 num_entities, List<ComponentRef<TestComponent>*>& component_refs) {
        List<Entity*> entity_set;
        for( uint32 i = 0; i < num_entities; i++ ) {
            entity_set.add(create_test_entity(component_refs));
        }
        return entity_set;
    }

    static void destroy_entity_set(List<Entity*>& entity_set) {
        for( uint32 i = 0; i < entity_set.get_size(); i++ ) {
            entity_set[i]->destroy();
        }
        entity_set.clear();
    }

    TD_TEST("entity-system/entity/chaotic-test") {
        TestComponent::reset_constructor_counters();
        
        {
            List<Entity*> all_entities;
            for( Entity* e : internal::Registry<Entity>::get_all() ) {
                all_entities.add(e);
            }
            for( uint32 i = 0; i < all_entities.get_size(); i++ ) {
                all_entities[i]->destroy();
            }
        }

        List<ComponentRef<TestComponent>*> component_refs;

        internal::Registry<Entity>::clear_block_list();

        TD_TEST_ASSERT_EQUAL(internal::Registry<Entity>::get_num_components(), 0U);

        List<Entity*> set_1 = create_entity_set(100, component_refs);

        TD_TEST_ASSERT_EQUAL(internal::Registry<Entity>::get_num_components(), 100U);

        List<Entity*> set_2 = create_entity_set(100, component_refs);

        TD_TEST_ASSERT_EQUAL(internal::Registry<Entity>::get_num_components(), 200U);

        destroy_entity_set(set_1);

        TD_TEST_ASSERT_EQUAL(internal::Registry<Entity>::get_num_components(), 100U);

        List<Entity*> set_3 = create_entity_set(50, component_refs);
        List<Entity*> set_4 = create_entity_set(50, component_refs);

        destroy_entity_set(set_3);  

        List<Entity*> set_5 = create_entity_set(50, component_refs);

        for( uint32 i = 0; i < component_refs.get_size(); i++ ) {
            ComponentRef<TestComponent>* ref = component_refs[i];
            if( (*ref) == nullptr ) continue; 
            TD_TEST_ASSERT_EQUAL((*ref)->a, 1);
            TD_TEST_ASSERT_EQUAL((*ref)->b, 2);
            TD_TEST_ASSERT_EQUAL((*ref)->c, 3);
        }

        for( uint32 i = 0; i < component_refs.get_size(); i++ ) {
            delete component_refs[i];
        }
    }

}