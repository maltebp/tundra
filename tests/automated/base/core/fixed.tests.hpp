#pragma once

#include "tundra/core/utility.hpp"
#include <tundra/core/log.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/test-framework/test.hpp>
#include <tundra/test-framework/test-assert.hpp>

// In general, these tests are somewhat incomplete - I'm adding as I encounter
// problems  and when I have some extra time to kill (like that ever happens)

TD_TEST("fixed/to-string/positive") {
    
    auto test_type = [&]<typename T>() {

        TD_TEST_ASSERT_EQUAL(td::to_string(T{}, 3), "0.000");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.25) }, 3), "0.250");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.125) }, 4), "0.1250");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.875) }, 6), "0.875000");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(1.125) }, 3), "1.125");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ 1 }, 3), "1.000");
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/to-string/negative") {

    auto test_type = [&]<typename T>() {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{}, 3), "0.000");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{}, 0), "0");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-0.25) }, 3), "-0.250");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-0.125) }, 4), "-0.1250");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-0.875) }, 4), "-0.8750");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-1.125) }, 3), "-1.125");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ -1 }, 3), "-1.000");
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
}

TD_TEST("fixed/to-string/rounding") {

    
    auto test_type = [&]<typename T>() {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.25) }, 1), "0.3");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.125) }, 2), "0.13");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.9453125) }, 0), "1");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.9453125) }, 1), "0.9");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.9453125) }, 2), "0.95");
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.9453125) }, 3), "0.945");
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/to-string/large-integer") {

    auto test_type = [&]<typename T>() {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{td::to_fixed(44031.9453125)}, 7), "44031.9453125");
    };

    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/to_string/leading-zeros") {
    
    auto test_type = [&]<typename T>() {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.0625) }, 4), "0.0625");
        
        if constexpr (td::is_signed<typename T::Type>() ) {
                TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-0.0625) }, 4), "-0.0625");
        }
    };
    
    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/to_string/smallest") {
    
    auto test_type = [&]<typename T>() {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(0.000244140625) }, 8), "0.00024414");
        
        if constexpr (td::is_signed<typename T::Type>() ) {
        TD_TEST_ASSERT_EQUAL(td::to_string(T{ td::to_fixed(-0.000244140625) }, 8), "-0.00024414");
        }
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/equality") {
    auto test_type = [&]<typename T>() {
        T f1 { 1 };
        T f2 { 1 };
        T f3 { 2 };
        TD_TEST_ASSERT_EQUAL(f1, f2);
        TD_TEST_ASSERT_NOT_EQUAL(f1, f3);
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/one") {
    {
        td::Fixed16<12> f1 { 1 };
        TD_TEST_ASSERT_EQUAL(f1.get_raw_value(), td::Fixed16<12>::ONE_RAW);
    }
}

TD_TEST("fixed/assignment/same-type") {
    
    auto test_type = [&]<typename T>() {
        T f1 { td::to_fixed(0.825) };
        T f2 { td::to_fixed(0.5) };
        TD_TEST_ASSERT_NOT_EQUAL(f1, f2);

        f2 = f1;
        TD_TEST_ASSERT_EQUAL(f1, f2);
    };

    // This is some cursed syntax, but it's how you call a templated lambda with no args
    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/assignment/different-type") {

    td::Fixed32<12> f32x12_1 { td::to_fixed(0.875) };

    // Different storage type
    td::Fixed16<12> f16x12_1 = td::Fixed16<12>(f32x12_1);
    TD_TEST_ASSERT_EQUAL(f16x12_1, f32x12_1);

    // Signed to unsigned
    td::UFixed16<12> uf16x12_1 = td::UFixed16<12>(f16x12_1);
    TD_TEST_ASSERT_EQUAL(f16x12_1, td::Fixed16<12>(uf16x12_1));

    // Different precision
    td::Fixed32<16> f32x16_1 = td::Fixed32<16>(f32x12_1);
    TD_TEST_ASSERT_EQUAL(f32x12_1, td::Fixed32<12>(f32x16_1));

    // Different precision and type
    td::Fixed16<4> f16_4 = td::Fixed16<4>(f32x12_1);
    TD_TEST_ASSERT_EQUAL(f32x12_1, td::Fixed32<12>(f16_4));
}

TD_TEST("fixed/addition") {

    auto test_type_unsigned = [&]<typename T>() {
        T f1 { td::to_fixed(0.5) };
        T f2 = f1 + 1;
        f2 += 1;
        f2 += f1;
        f2 += td::to_fixed(0.5);
        T expected { td::to_fixed(3.5) };
        TD_TEST_ASSERT_EQUAL(f2, expected);
    };

    test_type_unsigned.template operator()<td::Fixed16<12>>();
    test_type_unsigned.template operator()<td::UFixed16<12>>();
    test_type_unsigned.template operator()<td::Fixed32<12>>();
    test_type_unsigned.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/subtraction") {

    auto test_type_unsigned = [&]<typename T>() {
        T f1 { 4 };
        T f2 { td::to_fixed(0.5) };
        T result = f1 - 1;
        result -= 1;
        result -= f2;
        result -= td::to_fixed(0.5);
        T expected { td::to_fixed(1) };
        TD_TEST_ASSERT_EQUAL(result, expected);

        // Make sure it didn't change the others
        TD_TEST_ASSERT_EQUAL(f1, T(4));
        TD_TEST_ASSERT_EQUAL(f2, T(td::to_fixed(0.5)));
    };

    test_type_unsigned.template operator()<td::Fixed16<12>>();
    test_type_unsigned.template operator()<td::UFixed16<12>>();
    test_type_unsigned.template operator()<td::Fixed32<12>>();
    test_type_unsigned.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/multiplication/integer-signed") {

    auto test_type_signed = [&]<typename T>() {
        T f1 { td::to_fixed(0.5) };
        T f2 = f1 * -2;
        f2 *= 2;
        T expected { -2 };
        TD_TEST_ASSERT_EQUAL(f2, expected);
    };

    test_type_signed.template operator()<td::Fixed16<12>>();
    test_type_signed.template operator()<td::Fixed32<12>>();
}

TD_TEST("fixed/multiplication/integer-unsigned") {

    auto test_type_unsigned = [&]<typename T>() {
        T f1 { td::to_fixed(0.5) };
        T f2 = f1 * 2;
        f2 *= 2;
        T expected { 2 };
        TD_TEST_ASSERT_EQUAL(f2, expected);
    };

    test_type_unsigned.template operator()<td::Fixed16<12>>();
    test_type_unsigned.template operator()<td::UFixed16<12>>();
    test_type_unsigned.template operator()<td::Fixed32<12>>();
    test_type_unsigned.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/multiplication/double-signed") {

    auto test_type_signed = [&]<typename T>() {
        T f1 { 2 };
        T f2 = f1 * td::to_fixed(-0.5);
        f2 *= td::to_fixed(0.5);
        T expected { td::to_fixed(-0.5) };
        TD_TEST_ASSERT_EQUAL(f2, expected);
    };

    test_type_signed.template operator()<td::Fixed16<12>>();
    test_type_signed.template operator()<td::Fixed32<12>>();
}

TD_TEST("fixed/multiplication/double-unsigned") {
    auto test_type_unsigned = [&]<typename T>() {
        T f1 { 2 };
        T f2 = f1 * td::to_fixed(0.5);
        f2 *= td::to_fixed(0.5);
        T expected { td::to_fixed(0.5) };
        TD_TEST_ASSERT_EQUAL(f2, expected);
    };

    test_type_unsigned.template operator()<td::Fixed16<12>>();
    test_type_unsigned.template operator()<td::UFixed16<12>>();
    test_type_unsigned.template operator()<td::Fixed32<12>>();
    test_type_unsigned.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/multiplication/fixed-same") {

    auto test_type = [&]<typename T>() {
        T f1 { 2 };
        T f2 { td::to_fixed(0.5)};
        T result = f1 * f2;
        result *= f2;
        T expected { td::to_fixed(0.5) };
        TD_TEST_ASSERT_EQUAL(result, expected);
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/multiplication/fast") {

    auto test_type = [&]<typename T>() {
        T f1 { 2 };
        T f2 { td::to_fixed(0.5)};
        T result = f1.fast_multiply(f2);
        TD_TEST_ASSERT_EQUAL(result, T{1});
    };

    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/division/integer-signed") {

    auto test_type_signed = [&]<typename T>() {
        T f1 { 4 };
        T result = f1 / -2;
        result /= 2;
        TD_TEST_ASSERT_EQUAL(result, T{ -1 });
    };

    test_type_signed.template operator()<td::Fixed16<12>>();
    test_type_signed.template operator()<td::Fixed32<12>>();
}

TD_TEST("fixed/division/integer-unsigned") {

    auto test_type_unsigned = [&]<typename T>() {
        T f1 { 4 };
        T result = f1 / 2;
        result /= 2;
        TD_TEST_ASSERT_EQUAL(result, T{ 1 });
    };

    test_type_unsigned.template operator()<td::Fixed16<12>>();
    test_type_unsigned.template operator()<td::UFixed16<12>>();
    test_type_unsigned.template operator()<td::Fixed32<12>>();
    test_type_unsigned.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/division/double-signed") {
    auto test_type = [&]<typename T>() {
        T f1 { 1 };
        T result = f1 / td::to_fixed(-0.5);
        result /= td::to_fixed(0.5);
        TD_TEST_ASSERT_EQUAL(result, T{ -4 });
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
}

TD_TEST("fixed/division/double-unsigned") {

    auto test_type = [&]<typename T>() {
        T f1 { 1 };
        T result = f1 / td::to_fixed(0.5);
        result /= td::to_fixed(0.5);
        TD_TEST_ASSERT_EQUAL(result, T{ 4 });
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/division/fixed-same-type") {

    auto test_type = [&]<typename T>() {
        T f1 { 2 };
        T f2 { td::to_fixed(0.5)};
        T f3 { 4 };
        T result = f1 / f2;
        result /= f3;
        TD_TEST_ASSERT_EQUAL(result, T{1});
    };

    test_type.template operator()<td::Fixed16<12>>();
    test_type.template operator()<td::UFixed16<12>>();
    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

TD_TEST("fixed/division/fast") {

    auto test_type = [&]<typename T>() {
        T f1 { 4 };
        T f2 { td::to_fixed(2)};
        T result = f1.fast_divide(f2);
        TD_TEST_ASSERT_EQUAL(result, T{2});
    };

    test_type.template operator()<td::Fixed32<12>>();
    test_type.template operator()<td::UFixed32<12>>();
}

// Compile-time tests
//   Note: We can only really do positive tests with this setup

namespace fixed_compile_time_tests {

    namespace explicit_constructors {

        // Default constructors
        static constexpr td::Fixed16<12> f16_default{0};
        static constexpr td::UFixed16<12> uf16_default{0};
        static constexpr td::Fixed32<12> f32_default{0};
        static constexpr td::UFixed32<12> uf32_default{0};

        static constexpr td::UFixed16<12> uf16_f16 { f16_default };
        static constexpr td::Fixed32<12> f32_f16 { f16_default };
        static constexpr td::UFixed32<12> uf32_f16 { f16_default };
        static constexpr td::UFixed32<12> uf32_f32 { f32_default };

        // Different precision
        static constexpr td::Fixed32<16> f32x16_f32 { f32_default };
        static constexpr td::Fixed32<12> f32x12_f32x16 { f32x16_f32 };

        // From int
        static constexpr td::Fixed16<12> f16_int16{ (td::int16)0 };
        static constexpr td::UFixed16<12> uf16_uint16{ (td::uint16)0 };
        static constexpr td::Fixed32<12> f32_int32{ (td::int32)0 };
        static constexpr td::UFixed32<12> uf32_uint32{ (td::uint32)0 };

    }

    namespace assignment {

        static constexpr td::Fixed32<12> f32_implicit_int = []() {
            td::Fixed32<12> f;
            f = 1234;
            return f;
        }();

        static constexpr td::Fixed32<12> f32_f16 = []() {
            td::Fixed32<12> f;
            f = td::Fixed16<12>();
            return f;
        }();

        static constexpr td::Fixed32<12> f32_f32 = []() {
            td::Fixed32<12> f;
            f = td::Fixed32<12>();
            return f;
        }();

        // Should not be valid:
        //static constexpr td::Fixed16<12> f16x12_f16x14 = td::Fixed16<14>{};
        //static constexpr td::Fixed32<12> f32x12_f16x14 = td::Fixed16<14>{};
    }

    namespace addition {
        
        static_assert(td::Fixed32<12>(td::to_fixed(0.5)) + 2 == td::Fixed32<12>(td::to_fixed(2.5)));
        
        static_assert(td::Fixed16<12>(td::to_fixed(0.5)) + 2 == td::Fixed16<12>(td::to_fixed(2.5)));
    }

    namespace multiplication {

        static_assert(td::Fixed32<12>(td::to_fixed(0.5)) * 2 == td::Fixed32<12>(1));

        static_assert(td::Fixed32<12>(td::to_fixed(0.5)) * td::Fixed32<12>(td::to_fixed(0.5)) == td::Fixed32<12>(td::to_fixed(0.25)));

    }
}