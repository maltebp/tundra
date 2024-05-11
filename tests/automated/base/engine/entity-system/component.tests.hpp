#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/core/list.hpp>
#include <tundra/engine/entity-system/entity.hpp>
#include <tundra/engine/entity-system/component.hpp>


namespace td::component_tests {

    class TestComponent : public Component<TestComponent>, public ConstructorStatistics {
    public: 

        using ConstructorStatistics::ConstructorStatistics;

        int32 a { 1 };
        int16 b { 2};
        int8 c { 3};
    };

    TD_TEST("engine/entity-system/component/for-each") {
        TestComponent::reset_constructor_counters();

        List<TestComponent*> components;

        Entity* entity_1 = Entity::create();

        components.add(entity_1->add_component<TestComponent>());
        components.add(entity_1->add_component<TestComponent>());
        components.add(entity_1->add_component<TestComponent>());

        Entity* entity_2 = Entity::create();
        components.add(entity_2->add_component<TestComponent>());
        components.add(entity_2->add_component<TestComponent>());
        components.add(entity_2->add_component<TestComponent>());

        for( TestComponent* component : TestComponent::get_all() ) {
            TD_TEST_ASSERT_EQUAL(components.contains(component), true);        
            components.remove(component);
        }

        TD_TEST_ASSERT_EQUAL(components.get_size(), 0U);
    }

    TD_TEST("engine/entity-system/component/on-destroy") {
        static td::uint32 num_on_destroy_called = 0;
        
        class ComponentWithOnDestroy : public Component<ComponentWithOnDestroy> {
        public:
            virtual void on_destroy() override { num_on_destroy_called++; }
        };

        td::Entity* e = td::Entity::create();
        ComponentWithOnDestroy* c1 = e->add_component<ComponentWithOnDestroy>();
        e->add_component<ComponentWithOnDestroy>();

        TD_TEST_ASSERT_EQUAL(num_on_destroy_called, 0U);

        c1->destroy();
        TD_TEST_ASSERT_EQUAL(num_on_destroy_called, 1U);

        e->destroy();
        TD_TEST_ASSERT_EQUAL(num_on_destroy_called, 2U);
    }

}