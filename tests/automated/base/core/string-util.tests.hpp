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
        }

        {
            String s = string_util::create_from_format("numbers: %d, %d, %d", 1, 2, 3);
            TD_TEST_ASSERT_EQUAL(s, "numbers: 1, 2, 3");
        }

        {
            String s1 = "Inner String";
            String s2 = create_from_format("Outer String( %s )", s1.get_c_string());
            TD_TEST_ASSERT_EQUAL(s2, "Outer String( Inner String )");
        }
    }
}