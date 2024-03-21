#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/engine/entity-system/internal/registry-block.hpp>
#include <tundra/engine/entity-system/internal/component-meta-data.hpp>


namespace td::internal::registy_block_tests {

    class TestComponent : public ComponentMetaData {
    public:

        int16 a;
        int32 b;

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

    TD_TEST("entity-system/delete-and-recreate") {

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

}