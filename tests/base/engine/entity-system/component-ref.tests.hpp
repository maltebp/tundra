#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.hpp>
#include <tundra/engine/entity-system/entity.hpp>

namespace td::component_ref_tests {

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public: 

        int32 a { 1 };
        int16 b { 2};
        int8 c { 3};
    };
    
    TD_TEST("engine/entity-system/component-ref/basic") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        
        ComponentRef<TestComponent> c_ref = e->add_component<TestComponent>();

        TestComponent* c_deref;
        bool ref_is_valid = c_ref.get(c_deref);
        TD_TEST_ASSERT_EQUAL(ref_is_valid, true);
        TD_TEST_ASSERT_EQUAL(c_deref->a, 1);
        TD_TEST_ASSERT_EQUAL(c_deref->b, 2);
        TD_TEST_ASSERT_EQUAL(c_deref->c, 3);

        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, 1U);
    }

    TD_TEST("engine/entity-system/component-ref/reference-count-is-adjusted") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 0U);

        {
            ComponentRef<TestComponent> c_ref = c;
            TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 1U);
        }        

        TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 0);
    }

    TD_TEST("engine/entity-system/component-ref/when-component-is-destroyed-component-is-not-freed-until-all-references-are-released") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 0U);

        {
            ComponentRef<TestComponent> c_ref = c;
            TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 1U);

            c->destroy();
            TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);
            TD_TEST_ASSERT_EQUAL(e->get_num_components(), 0);
        }        

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 1U);

        e->destroy();
    }

    TD_TEST("engine/entity-system/component-ref/multiple-references") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 0U);

        {
            ComponentRef<TestComponent> c_ref_1 = c;
            {
                ComponentRef<TestComponent> c_ref_2 = c;

                TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 2U);

                c->destroy();
                TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);
                TD_TEST_ASSERT_EQUAL(e->get_num_components(), 0);
            }
            
            TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 1U);

            TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);
        }        

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 1U);

        e->destroy();
    }

    TD_TEST("engine/entity-system/component-ref/when-entity-is-destroyed-component-is-not-freed-until-all-references-are-released") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();
        TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 0U);

        {
            ComponentRef<TestComponent> c_ref = c;
            TD_TEST_ASSERT_EQUAL(c->get_reference_count(), 1U);

            e->destroy();
            TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);
        }        

        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 1U);
    }

    TD_TEST("engine/entity-system/component-ref/with-multiple-components-entity-is-valid-given-one-has-non-zero-ref-count") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();

        e->add_component<TestComponent>();
        TestComponent* c_2 = e->add_component<TestComponent>();
        e->add_component<TestComponent>();

        {
            ComponentRef<TestComponent> c_ref = c_2;
            c_2->destroy();
            TD_TEST_ASSERT_EQUAL(e->get_num_components(), 2);            
        }        

        e->destroy();
    }

}