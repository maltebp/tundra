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
        internal::Registry<TestComponent>::clear_block_list();
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
        internal::Registry<TestComponent>::clear_block_list();

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

    TD_TEST("engine/entity-system/registry/iterator/empty") {
        internal::Registry<TestComponent>::clear_block_list();

        TD_ASSERT(internal::Registry<TestComponent>::get_num_components() == 0, "Registry was not clean when entering test");
        
        int num_iterated_components = 0;
        for( TestComponent* component : internal::Registry<TestComponent>::get_iterable() ) {
            static_cast<void>(component);
            num_iterated_components++;
        }
        TD_TEST_ASSERT_EQUAL(num_iterated_components, 0);
    }

    TD_TEST("engine/entity-system/registry/iterator/one-full-block") {
        internal::Registry<TestComponent>::clear_block_list();

        List<TestComponent*> components;
        for( int i = 0; i < 10; i++ ) {
            components.add(internal::Registry<TestComponent>::create_component());
        }

        int num_iterated_components = 0;
        for( TestComponent* component : internal::Registry<TestComponent>::get_iterable() ) {
            num_iterated_components++;
            uint32 index_of_component = components.index_of(component);
            TD_TEST_ASSERT_LESS(index_of_component, td::limits::numeric_limits<uint32>::max);
        }

        TD_TEST_ASSERT_EQUAL(num_iterated_components, 10);

        // This is just to not pollute other tests (it's a bit unfortunate)
        for( uint32 i = 0; i < components.get_size(); i++ ) {
            internal::Registry<TestComponent>::destroy_component(components[i]);
        }
    }

    TD_TEST("engine/entity-system/registry/iterator/multiple-full-blocks") {
        internal::Registry<TestComponent>::clear_block_list();

        using Reg = internal::Registry<TestComponent>;
        const uint32 NUM_BLOCKS = 3;
        const uint32 NUM_COMPONENTS = Reg::BLOCK_SIZE * NUM_BLOCKS;

        List<TestComponent*> components;
        for( uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
            components.add(Reg::create_component());
        }

        uint32 num_iterated_components = 0;
        for( TestComponent* component : internal::Registry<TestComponent>::get_iterable() ) {
            num_iterated_components++;
            uint32 index_of_component = components.index_of(component);
            TD_TEST_ASSERT_LESS(index_of_component, td::limits::numeric_limits<uint32>::max);
        }

        TD_TEST_ASSERT_EQUAL(num_iterated_components, NUM_COMPONENTS);

         // This is just to not pollute other tests (it's a bit unfortunate)
        for( uint32 i = 0; i < components.get_size(); i++ ) {
            internal::Registry<TestComponent>::destroy_component(components[i]);
        }
    }

    TD_TEST("engine/entity-system/registry/iterator/multiple-empty-blocks") {
        internal::Registry<TestComponent>::clear_block_list();

        using Reg = internal::Registry<TestComponent>;
        const uint32 NUM_BLOCKS = 3;
        const uint32 NUM_COMPONENTS = Reg::BLOCK_SIZE * NUM_BLOCKS;

        List<TestComponent*> components;
        for( uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
            components.add(Reg::create_component());
        }

        for( uint32 i = 0; i < components.get_size(); i++ ) {
            internal::Registry<TestComponent>::destroy_component(components[i]);
        }

        components.clear();

        int num_iterated_components = 0;
        for( TestComponent* component : internal::Registry<TestComponent>::get_iterable() ) {
            static_cast<void>(component);
            num_iterated_components++;
        }
        TD_TEST_ASSERT_EQUAL(num_iterated_components, 0);
    }

    TD_TEST("engine/entity-system/registry/iterator/some-empty-blocks") {
        internal::Registry<TestComponent>::clear_block_list();
        
        TD_ASSERT(internal::Registry<TestComponent>::get_num_components() == 0, "Registry was not clean when entering test");

        using Reg = internal::Registry<TestComponent>;
        const uint32 NUM_BLOCKS = 4;
        const uint32 NUM_COMPONENTS = Reg::BLOCK_SIZE * NUM_BLOCKS;

        List<TestComponent*> components;
        for( uint32 i = 0; i < NUM_COMPONENTS; i++ ) {
            components.add(Reg::create_component());
        }

        auto remove_all_from_block = [&components](uint32 block_index) {
            for( uint32 i = Reg::BLOCK_SIZE * block_index; i < Reg::BLOCK_SIZE * (block_index + 1); i++ ) {
                internal::Registry<TestComponent>::destroy_component(components[i]);
            } 
        };

        remove_all_from_block(0);
        remove_all_from_block(2);

        uint32 num_iterated_components = 0;
        for( TestComponent* component : internal::Registry<TestComponent>::get_iterable() ) {
            static_cast<void>(component);
            num_iterated_components++;
        }
        TD_TEST_ASSERT_EQUAL(num_iterated_components, Reg::BLOCK_SIZE * 2);

        remove_all_from_block(1);
        remove_all_from_block(3);

        TD_ASSERT(internal::Registry<TestComponent>::get_num_components() == 0, "Test didn't clean up properly");
    }
}