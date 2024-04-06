#pragma once

#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

#include <tundra/core/string.hpp>
#include <tundra/core/string-util.hpp>

namespace td::string_util::tests {

    TD_TEST("string-util/create-from-format") {

        {
            String s = string_util::create_from_format("Hello, %s!", "world");
            TD_TEST_ASSERT_EQUAL(s, "Hello, world!");
            
            char last_char = s.get_c_string()[s.get_size()];
            TD_TEST_ASSERT_EQUAL(last_char, '\0');
        }

        {
            String s = string_util::create_from_format("numbers: %d, %d, %d", 1, 2, 3);
            TD_TEST_ASSERT_EQUAL(s, "numbers: 1, 2, 3");
            
            char last_char = s.get_c_string()[s.get_size()];
            TD_TEST_ASSERT_EQUAL(last_char, '\0');
        }

        {
            String s1 = "Inner String";
            String s2 = create_from_format("Outer String( %s )", s1.get_c_string());
            TD_TEST_ASSERT_EQUAL(s2, "Outer String( Inner String )");

            char last_char = s2.get_c_string()[s2.get_size()];
            TD_TEST_ASSERT_EQUAL(last_char, '\0');
        }

        {
            String s1 = create_from_format("This is a very, very, very, very long string");
            TD_TEST_ASSERT_EQUAL(s1, "This is a very, very, very, very long string");

            char s1_last_char = s1.get_c_string()[s1.get_size()];
            TD_TEST_ASSERT_EQUAL(s1_last_char, '\0');

            String s2 = create_from_format("This is a not so long string");
            TD_TEST_ASSERT_EQUAL(s2, "This is a not so long string");

            char s2_last_char = s2.get_c_string()[s2.get_size()];
            TD_TEST_ASSERT_EQUAL(s2_last_char, '\0');
        }
    }
}