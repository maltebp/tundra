#pragma once

#include "tundra/core/string-util.hpp"
#include "tundra/core/string.hpp"
#include "tundra/core/utility.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/list.hpp>


class TestType {
public:

    TestType() {
        num_default_constructors_called++;
        num_constructors_called++;
    }

    TestType(int value) : value(value) {
        num_constructors_called++;
    }

    TestType(const TestType& other) 
        :   value(other.value) 
    {
        num_copy_constructors_called++;
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

    static void reset_constructor_counters() {
        num_default_constructors_called = 0;
        num_copy_constructors_called = 0;
        num_constructors_called = 0;
        num_move_constructors_called = 0;
        num_destructors_called = 0;
    }

    int value = 0;

    static inline td::uint32 num_default_constructors_called = 0;
    static inline td::uint32 num_copy_constructors_called = 0;
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
    
    td::List<TestType> list;
    list.add(TestType{1});
    list.add(TestType{2});
    list.add(TestType{3});
    list.add(TestType{4});

    TestType to_remove {4};

    TestType::reset_constructor_counters();
    
    bool removed = list.remove(to_remove);
    TD_TEST_ASSERT_EQUAL(removed, true);
    TD_TEST_ASSERT_EQUAL(list.get_size(), 3U);       
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 1U);

    TD_TEST_ASSERT_EQUAL(list[0], TestType{1});
    TD_TEST_ASSERT_EQUAL(list[1], TestType{2});
    TD_TEST_ASSERT_EQUAL(list[2], TestType{3});
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

    list.remove_at(1);

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 1U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 2U);
    TD_TEST_ASSERT_EQUAL(list.get_size(), 2U);       
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

TD_TEST("list/size-constructor") {

    TestType::reset_constructor_counters();

    td::List<TestType> list_1;
    TD_TEST_ASSERT_EQUAL(list_1.get_size(), 0U);

    td::List<TestType> list_2 { 3 };
    TD_TEST_ASSERT_EQUAL(list_2.get_size(), 3U);
    TD_TEST_ASSERT_EQUAL(TestType::num_default_constructors_called, 3U);
    
    TD_TEST_ASSERT_EQUAL(list_2[0], TestType{});
    TD_TEST_ASSERT_EQUAL(list_2[1], TestType{});
    TD_TEST_ASSERT_EQUAL(list_2[2], TestType{});
}

TD_TEST("list/copy-constructor") {
    
    td::List<TestType> list;
    list.add({1});
    list.add({2});
    list.add({3});

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;
    TestType::num_move_constructors_called = 0;

    td::List<TestType> list_copy_1 { list };

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 3U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 0U);

    TD_TEST_ASSERT_EQUAL(list_copy_1.get_size(), 3U);   
    TD_TEST_ASSERT_EQUAL(list_copy_1[0], TestType{1});   
    TD_TEST_ASSERT_EQUAL(list_copy_1[1], TestType{2});   
    TD_TEST_ASSERT_EQUAL(list_copy_1[2], TestType{3});   
}

TD_TEST("list/copy-constructor-empty") {

    TestType::num_constructors_called = 0;
    TestType::num_destructors_called = 0;
    TestType::num_move_constructors_called = 0;

    td::List<TestType> list;
    td::List<TestType> list_copy { list };

    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(list_copy.get_size(), 0U);   
}

TD_TEST("list/copy-assignment") {
 
    td::List<TestType> list_1;
    list_1.add({1});
    list_1.add({2});
    list_1.add({3});

    TestType::reset_constructor_counters();

    td::List<TestType> list_2;
    TD_TEST_ASSERT_EQUAL(list_2.get_size(), 0U);

    list_2 = list_1;
    TD_TEST_ASSERT_EQUAL(list_2.get_size(), 3U);
    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 3U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 0U);

    td::List<TestType> list_3;
    list_3.add(4);
    TestType::reset_constructor_counters();

    list_3 = list_2;
    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 3U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 1U);

    // Assign empty list
    TestType::reset_constructor_counters();
    td::List<TestType> list_4;
    list_3 = list_4;
    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 3U);
}

TD_TEST("list/move-constructor") {

    td::List<TestType> list_1;
    list_1.add({1});
    list_1.add({2});
    list_1.add({3});

    TestType::reset_constructor_counters();

    td::List<TestType> list_2 { td::move(list_1) };
    
    // We simply move the pointer of the allocated memory, so no constructors
    // nor destructors should be called
    TD_TEST_ASSERT_EQUAL(TestType::num_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_move_constructors_called, 0U);
    TD_TEST_ASSERT_EQUAL(TestType::num_destructors_called, 0U);

    TD_TEST_ASSERT_EQUAL(list_2.get_size(), 3U);
    TD_TEST_ASSERT_EQUAL(list_2[0], TestType{1});
    TD_TEST_ASSERT_EQUAL(list_2[1], TestType{2});
    TD_TEST_ASSERT_EQUAL(list_2[2], TestType{3});
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