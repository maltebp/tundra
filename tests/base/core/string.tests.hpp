#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/string.hpp>


TD_TEST("string/basic-constructor") {
    
    const char* c_string = "test";
    td::String s {c_string};

    // Ensure that memory was copied
    TD_TEST_ASSERT_NOT_EQUAL(s.get_c_string(), c_string);

    TD_TEST_ASSERT_EQUAL(s.get_size(), 4);

    int i = 0;
    while(c_string[i] != '\0') {
        TD_TEST_ASSERT_EQUAL(s.get_c_string()[i], c_string[i]);
        i++;
    }
}

TD_TEST("string/equals-operator") {
    
    const char* c_string_1 = "test";
    const char* c_string_2 = "test2";
    td::String s1 { c_string_1 };
    td::String s2 { c_string_1 };
    td::String s3 { c_string_2 };
    td::String s4 { };

    TD_TEST_ASSERT_EQUAL(s1, c_string_1);
    TD_TEST_ASSERT_EQUAL(s1, s2);

    TD_TEST_ASSERT_EQUAL(s1, s1);
    TD_TEST_ASSERT_EQUAL(s2, s2);
    TD_TEST_ASSERT_EQUAL(s3, s3);
    TD_TEST_ASSERT_EQUAL(s4, s4);

    TD_TEST_ASSERT_NOT_EQUAL(s1, s3);
    TD_TEST_ASSERT_NOT_EQUAL(s1, c_string_2);
    TD_TEST_ASSERT_NOT_EQUAL(s1, s4);
    TD_TEST_ASSERT_NOT_EQUAL(s1, "");
}

TD_TEST("string/constructors") {
    
    td::String test { "test" };   
    TD_TEST_ASSERT_EQUAL(test, "test");

    td::String empty;
    TD_TEST_ASSERT_EQUAL(empty, "");

    td::String test_copy { test };
    TD_TEST_ASSERT_EQUAL(test_copy, test);

    td::String copy_assigned { "not assigned" };
    
    copy_assigned = test;
    TD_TEST_ASSERT_EQUAL(copy_assigned, test);

    copy_assigned = "";
    TD_TEST_ASSERT_EQUAL(copy_assigned, "");

    copy_assigned = "test2";
    TD_TEST_ASSERT_EQUAL(copy_assigned, "test2");

    copy_assigned = empty;
    TD_TEST_ASSERT_EQUAL(copy_assigned, empty);
}

TD_TEST("string/append") {
    
    td::String test { "test" };   
    
    test += "1";
    TD_TEST_ASSERT_EQUAL(test, "test1");

    td::String other_string { "other" };
    test += other_string;
    TD_TEST_ASSERT_EQUAL(test, "test1other");
    TD_TEST_ASSERT_EQUAL(other_string, "other");

    td::String empty_string;
    test += empty_string;
    TD_TEST_ASSERT_EQUAL(test, "test1other");

    empty_string += test;
    TD_TEST_ASSERT_EQUAL(empty_string, "test1other");
}

TD_TEST("string/add") {

    td::String test { "test" };

    TD_TEST_ASSERT_EQUAL(test + "1", "test1");

    TD_TEST_ASSERT_EQUAL(test + test, "testtest");

    TD_TEST_ASSERT_EQUAL(test + "", "test");
}