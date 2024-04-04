#pragma once


#include "tundra/core/fixed.hpp"
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>
#include <tundra/test-framework/utility/constructor-statistics.hpp>

#include <tundra/core/math.hpp>

namespace td {

    TD_TEST("math/radians_to_revolutions") {

        // This error is too high I think
        constexpr td::Fixed32<12> ALLOWED_ERROR = to_fixed(0.00390625); // 2^-8

        td::Fixed32<12> result_difference;

        result_difference = radians_to_revolutions(td::Fixed32<12>{0}); 
        TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
        TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);

        result_difference = radians_to_revolutions(td::Fixed32<12>::get_half_pi()) - to_fixed(0.25);
        TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
        TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);

        result_difference = radians_to_revolutions(td::Fixed32<12>::get_pi()) - to_fixed(0.5); 
        TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
        TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);

        result_difference = radians_to_revolutions(td::Fixed32<12>::get_pi() * 2) - to_fixed(1);
        TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
        TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);
    }

    TD_TEST("atan2") {

        // This error is too high I think
        constexpr td::Fixed32<12> ALLOWED_ERROR = to_fixed(0.00390625);

        {
            td::Fixed32<12> x {1};
            td::Fixed32<12> y {0};
            td::Fixed32<12> result_difference = atan2(y, x) - 0; 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);
        }

        {
            td::Fixed32<12> x {0};
            td::Fixed32<12> y {1};
            td::Fixed32<12> result_difference = atan2(y, x) - to_fixed(0.25); 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);
        }

        {
            td::Fixed32<12> x {-1};
            td::Fixed32<12> y {0};
            td::Fixed32<12> result_difference = atan2(y, x) - to_fixed(0.5); 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);
        }

        {
            td::Fixed32<12> x {0};
            td::Fixed32<12> y {-1};
            td::Fixed32<12> result_difference = atan2(y, x) - to_fixed(-0.25); 
            TD_TEST_ASSERT_GREATER_OR_EQUAL(result_difference, -ALLOWED_ERROR);
            TD_TEST_ASSERT_LESS_OR_EQUAL(result_difference, ALLOWED_ERROR);
        }
    }

}