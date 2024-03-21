#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/engine/entity-system/component.hpp>
#include <tundra/engine/entity-system/internal/registry.hpp>


namespace td::registry_tests {

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public:

        int16 a = 1;
        int32 b = 2;

    };   

    TD_TEST("engine/entity-system/registry/basic") {
        TestComponent::reset_constructor_counters();

        TestComponent* c = internal::Registry<TestComponent>::create_component();

        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, 1U);
        TD_TEST_ASSERT_EQUAL(internal::Registry<TestComponent>::get_num_components(), 1U);

        TD_TEST_ASSERT_EQUAL(c->a, 1);
        TD_TEST_ASSERT_EQUAL(c->b, 2);

        internal::Registry<TestComponent>::destroy_component(c);
        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 1U);        
    }

    TD_TEST("engine/entity-system/registry/multiple-blocks") {

        using Reg = internal::Registry<TestComponent>;
        const uint32 NUM_BLOCKS = 3;
        const uint32 NUM_COMPONENTS = Reg::BLOCK_SIZE * NUM_BLOCKS;

        TestComponent::reset_constructor_counters();
        
        List<TestComponent*> components;
        for( uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
            components.add(Reg::create_component());
        }

        TD_TEST_ASSERT_EQUAL(Reg::get_num_components(), NUM_COMPONENTS);
        TD_TEST_ASSERT_EQUAL(Reg::get_num_blocks(), NUM_BLOCKS);

        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, NUM_COMPONENTS);
        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, 0U);

        for( uint32 i = 0; i < components.get_size(); i++ ) {
            TD_TEST_ASSERT_EQUAL(components[i]->a, 1);
            TD_TEST_ASSERT_EQUAL(components[i]->b, 2);
        }

        for( uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
            Reg::destroy_component(components[i]);
        }

        TD_TEST_ASSERT_EQUAL(TestComponent::num_constructors_called, NUM_COMPONENTS);
        TD_TEST_ASSERT_EQUAL(TestComponent::num_destructors_called, NUM_COMPONENTS);
        TD_TEST_ASSERT_EQUAL(Reg::get_num_components(), 0U);
        TD_TEST_ASSERT_EQUAL(Reg::get_num_blocks(), NUM_BLOCKS);      
    }

}