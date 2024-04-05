#pragma once

#include "tundra/core/assert.hpp"
#include "tundra/core/string-util.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.hpp>
#include <tundra/engine/entity-system/entity.hpp>

namespace td::component_ref_tests {
    class TestComponent;
}

template<>
td::String td::to_string<td::component_ref_tests::TestComponent>(const td::component_ref_tests::TestComponent& c);

namespace td::component_ref_tests {
    

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public: 

        int32 a { 1 };
        int16 b { 2};
        int8 c { 3};

        constexpr bool operator==(const TestComponent& other) const { return a == other.a && b == other.b && c == other.c; }
    };
    
    TD_TEST("engine/entity-system/component-ref/basic") {
        TestComponent::reset_constructor_counters();
        
        Entity* e = Entity::create();
        
        TestComponent* c = e->add_component<TestComponent>();
        ComponentRef<TestComponent> c_ref = c;

        TestComponent* c_deref = c_ref;
        TD_TEST_ASSERT_EQUAL(c_deref, c);
        TD_TEST_ASSERT_EQUAL(&*c_ref, c);

        TD_TEST_ASSERT_EQUAL(c_deref->a, 1);
        TD_TEST_ASSERT_EQUAL(c_deref->b, 2);
        TD_TEST_ASSERT_EQUAL(c_deref->c, 3);

        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, 1U);
    }

    TD_TEST("engine/entity-system/component-ref/copy-constructor") {
        TestComponent::reset_constructor_counters();
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();

        ComponentRef<TestComponent> c_ref_1 = c;
        ComponentRef<TestComponent> c_ref_2 = c_ref_1;

        TD_TEST_ASSERT_EQUAL(c_ref_1, c_ref_2);

        TD_TEST_ASSERT_EQUAL(*c_ref_1, TestComponent{});
        TD_TEST_ASSERT_EQUAL(*c_ref_2, TestComponent{});
    }

    TD_TEST("engine/entity-system/component-ref/equality") {
        Entity* e = Entity::create();
        TestComponent* c = e->add_component<TestComponent>();
        
        ComponentRef<TestComponent> ref_1 { c };
        ComponentRef<TestComponent> ref_2 { ref_1 };
        ComponentRef<TestComponent> ref_3;

        TD_TEST_ASSERT_EQUAL(ref_1, c);
        TD_TEST_ASSERT_EQUAL(ref_2, c);

        TD_TEST_ASSERT_EQUAL(ref_1, ref_2);
        
        TD_TEST_ASSERT_NOT_EQUAL(ref_3, ref_1);
        TD_TEST_ASSERT_NOT_EQUAL(ref_3, ref_2);
        TD_TEST_ASSERT_NOT_EQUAL(ref_3, c);
        TD_TEST_ASSERT_EQUAL(ref_3, nullptr);
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

template<>
td::String td::to_string<td::component_ref_tests::TestComponent>(const td::component_ref_tests::TestComponent& c) {
    return td::string_util::create_from_format("TestComponent{%d, %d, %d}", c.a, c.b, c.c);
}