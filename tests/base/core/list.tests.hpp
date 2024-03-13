#pragma once

#include "tundra/core/string-util.hpp"
#include "tundra/core/string.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/list.hpp>


class TestType {
public:

    TestType() {
        num_constructors_called++;
    }

    TestType(int value) : value(value) {
        num_constructors_called++;
    }

    TestType(const TestType& other) 
        :   value(other.value) 
    {
        num_constructors_called++;
    }

    TestType(TestType&& other) 
        :   value(other.value) 
    {
        num_constructors_called++;
        num_move_constructors_called++;
    }

    ~TestType() {
        num_destructors_called++;
    }

    bool operator==(const TestType& other) const {
        return value == other.value;
    }

    bool operator!=(const TestType& other) const {
        return value != other.value;
    }

    int value = 0;

    static inline td::uint32 num_constructors_called = 0;
    static inline td::uint32 num_move_constructors_called = 0;
    static inline td::uint32 num_destructors_called = 0;
};

template<>
td::String td::to_string<TestType>(const TestType& t) {
    return td::string_util::create_from_format("TestType(%d)", t.value);
}

TD_TEST("list/add") {
    
    TestType t1 { 10 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;

    td::List<TestType> list;
    list.add(t1);

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 0U);

    TD_TEST_ASSERT_EQUAL(list[0], TestType{10});    
    TD_TEST_ASSERT_EQUAL(list.get_size(), 1U);   
}

TD_TEST("list/add-and-remove") {
    
    TestType t1 { 10 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;

    td::List<TestType> list;
    list.add(t1);
    list.remove(0);

    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(list.get_size(), 0U);       
}

TD_TEST("list/remove-in-middle") {
    
    TestType t1 { 10 };
    TestType t2 { 15 };
    TestType t3 { 20 };

    td::List<TestType> list;
    list.add(t1);
    list.add(t2);
    list.add(t3);

    TestType::num_constructors_called = 0;
    TestType::num_move_constructors_called = 0;
    TestType::num_destructors_called = 0;

    list.remove(1);

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 2U);
    TD_TEST_ASSERT_EQUAL(list.get_size(), 2U);       
}

TD_TEST("list/destructor") {
    TestType t1 { 10 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;

    {
        td::List<TestType> list;
        list.add(t1);    
    }
    
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 1U);
}

TD_TEST("list/clear") {
    
    TestType t1 { 10 };
    TestType t2 { 15 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;

    td::List<TestType> list;
    list.add(t1);
    list.add(t2);

    list.clear();

    TD_TEST_ASSERT_EQUAL(list.get_size(), 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 2U);
}

TD_TEST("list/clear-reuse") {
    
    TestType t1 { 10 };
    TestType t2 { 15 };
    TestType t3 { 20 };
    TestType t4 { 25 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;

    td::List<TestType> list;
    list.add(t1);
    list.add(t2);
    list.clear();

    list.add(t1);
    list.add(t2);
    
    TD_TEST_ASSERT_EQUAL(list.get_size(), 2U);
    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 4U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 2U);
}

TD_TEST("list/add-with-relocation") {

    TestType t1 { 10 };
    TestType t2 { 15 };
    TestType t3 { 20 };

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;
    TestType::num_move_constructors_called = 0;

    td::List<TestType> list;
    list.add(t1);
    list.add(t2);

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 2U);

    list.add(t3);

    TD_TEST_ASSERT_EQUAL(list.get_size(), 3U);

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 5U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 2U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 2U);

    TD_TEST_ASSERT_EQUAL(list[0], TestType{10});
    TD_TEST_ASSERT_EQUAL(list[1], TestType{15});
    TD_TEST_ASSERT_EQUAL(list[2], TestType{20});
}