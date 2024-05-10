#pragma once

#include "tundra/core/assert.hpp"
#include "tundra/core/limits.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/engine/entity-system/internal/registry-block.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>


namespace td::internal::registy_block_tests {

    class TestComponent : public ComponentMetaData {
    public:

        int16 a = 1;
        int32 b = 2;

    };   

    TD_TEST("entity-system/registry-block/create-and-delete") {

        RegistryBlock<TestComponent> registry_block{2};

        TestComponent* component_1 = registry_block.allocate_component();
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), 1);

        TestComponent* component_2 = registry_block.allocate_component();
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), 2);

        TD_TEST_ASSERT_NOT_EQUAL(component_1, component_2);

        registry_block.free_component(component_2);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), 1);

        registry_block.free_component(component_1);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), 0);
    }

    TD_TEST("entity-system/registry-block/extending-hole") {

        RegistryBlock<TestComponent> registry_block{3};

        TestComponent* component_1;
        TestComponent* component_2;
        TestComponent* component_3;

        component_1 = registry_block.allocate_component();
        component_2 = registry_block.allocate_component();
        component_3 = registry_block.allocate_component();

        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 0);

        registry_block.free_component(component_1);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(component_2);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(component_3);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        component_1 = registry_block.allocate_component();
        component_2 = registry_block.allocate_component();
        component_3 = registry_block.allocate_component();

        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 0);

        // Deleting in opposite direction
        registry_block.free_component(component_3);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(component_2);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(component_1);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);
    }

    TD_TEST("entity-system/registry-block/merging-hole") {

        RegistryBlock<TestComponent> registry_block{3};

        TestComponent* component_1 = registry_block.allocate_component();
        TestComponent* component_2 = registry_block.allocate_component();
        TestComponent* component_3 = registry_block.allocate_component();

        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 0);

        registry_block.free_component(component_1);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(component_3);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 2);

        registry_block.free_component(component_2);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);
    }

    TD_TEST("entity-system/registry-block/delete-and-recreate") {

        RegistryBlock<TestComponent> registry_block{10};

        List<TestComponent*> allocated_components;

        for( uint16 i = 0; i < 10; i++ ) {
            allocated_components.add(registry_block.allocate_component());
            TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), i + 1);
        }

        // Delete every other
        for( int16 i = 4; i >= 0; i-- ) {
            registry_block.free_component(allocated_components[(uint32)i * 2]); 
            allocated_components.remove_at((uint32)i * 2);
            TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), allocated_components.get_size());
            TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 5 - i);
        }

        for( uint16 i = 0; i < 5; i++ ) {
            allocated_components.add(registry_block.allocate_component());
            TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), allocated_components.get_size());
        }

        for( uint16 i = 0; i < 10; i++ ) {
            TestComponent* component = allocated_components[i];
            uint16 index_of_component = registry_block.get_index_of_component(component);
            TD_TEST_ASSERT_GREATER_OR_EQUAL(index_of_component, 0);
            TD_TEST_ASSERT_LESS(index_of_component, 10);
            registry_block.free_component(component);
        }

        TD_TEST_ASSERT_EQUAL(registry_block.get_num_allocated_components(), 0);
    }

    TD_TEST("entity-system/registry-block/iterator/no-holes") {

        RegistryBlock<TestComponent> registry_block{10};

        List<TestComponent*> allocated_components;
        for( uint16 i = 0; i < 10; i++ ) {
            allocated_components.add(registry_block.allocate_component());
        }

        int num_iterated_components = 0;
        for( TestComponent* component : registry_block ) {
            num_iterated_components++;
            uint32 index_of_component = allocated_components.index_of(component);
            TD_TEST_ASSERT_LESS(index_of_component, td::limits::numeric_limits<uint32>::max);
        }

        TD_TEST_ASSERT_EQUAL(num_iterated_components, 10);
    }

    TD_TEST("entity-system/registry-block/iterator/empty") {

        RegistryBlock<TestComponent> registry_block{10};

        int num_iterated_components = 0;
        for( TestComponent* component : registry_block ) {
            static_cast<void>(component);
            num_iterated_components++;
        }

        TD_TEST_ASSERT_EQUAL(num_iterated_components, 0);
    }

    TD_TEST("entity-system/registry-block/iterator/holes") {

        RegistryBlock<TestComponent> registry_block{10};

        List<TestComponent*> allocated_components;
        for( uint16 i = 0; i < 10; i++ ) {
            allocated_components.add(registry_block.allocate_component());
        }

        // Delete every other (starting from the first in allocated memory - i.e.
        // the 10th allocated component)
        for( uint32 i = 0; i < 5; i++ ) {
            registry_block.free_component(allocated_components[i * 2 + 1]);
        }

        int num_iterated_components = 0;
        for( TestComponent* component : registry_block ) {
            num_iterated_components++;
            uint32 index_of_component = allocated_components.index_of(component);
            TD_TEST_ASSERT_LESS(index_of_component, td::limits::numeric_limits<uint32>::max);
        }

        TD_TEST_ASSERT_EQUAL(num_iterated_components, 5);
    }

     TD_TEST("entity-system/registry-block/merging-holes") {
        // This is a minimal reproducible example of a bug I ran into at some point

        // The original bug: 
        // Freeing a component, where previous entry is a single-entry hole,
        // it would set the newly freed component (which is the new tail), to
        // point to itself as the head of the hole it belongs to.

        RegistryBlock<TestComponent> registry_block{5};
        
        // It has to be 4 components (with only 3, it would not happen)
        TestComponent* c1 = registry_block.allocate_component();
        TestComponent* c2 = registry_block.allocate_component();
        TestComponent* c3 = registry_block.allocate_component();
        TestComponent* c4 = registry_block.allocate_component();
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);

        registry_block.free_component(c2);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 2);

        registry_block.free_component(c1);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 2);

        // Original bug: This would cause an assertion, complaining 
        // there is no hole starting at index 4
        registry_block.free_component(c3); 
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 2);

        registry_block.free_component(c4);
        TD_TEST_ASSERT_EQUAL(registry_block.get_num_holes(), 1);
    }

}