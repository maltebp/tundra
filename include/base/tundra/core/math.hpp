#pragma once

#include "tundra/core/log.hpp"
#include "tundra/core/math.hpp"
#include <type_traits>

#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>

namespace td {

    // From https://github.com/MikeLankamp/fpm/blob/master/include/fpm/math.hpp
    // fitted to td::Fixed use 

    // Source license:
    // MIT License

    // Copyright (c) 2019 Mike Lankamp

    // Permission is hereby granted, free of charge, to any person obtaining a copy
    // of this software and associated documentation files (the "Software"), to deal
    // in the Software without restriction, including without limitation the rights
    // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    // copies of the Software, and to permit persons to whom the Software is
    // furnished to do so, subject to the following conditions:

    // The above copyright notice and this permission notice shall be included in all
    // copies or substantial portions of the Software.

    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    // SOFTWARE.

    namespace internal {

        // Returns the index of the most-signifcant set bit
        inline unsigned long find_highest_bit(unsigned long long value) noexcept
        {
            TD_ASSERT(value != 0, "Value must not be 0");
            #if defined(_MSC_VER)
                unsigned long index;
            #if defined(_WIN64)
                _BitScanReverse64(&index, value);
            #else
                if (_BitScanReverse(&index, static_cast<unsigned long>(value >> 32)) != 0) {
                    index += 32;
                } else {
                    _BitScanReverse(&index, static_cast<unsigned long>(value & 0xfffffffflu));
                }
            #endif
                return index;
            #elif defined(__GNUC__) || defined(__clang__)
                return sizeof(value) * 8 - 1 - (size_t)__builtin_clzll(value);
            #else
            #   error "your platform does not support find_highest_bit()"
            #endif
        }

        // Calculates atan(x) assuming that x is in the range [0,1]
        template<typename TFixed>
        [[nodiscard]] constexpr TFixed atan_sanitized(TFixed x) 
            noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
        {

            TD_ASSERT(x >= TFixed{to_fixed(0)} && x <= TFixed{to_fixed(1)}, "Fixed was not between 0 and 1 (was %s)", to_string(x, 6));

            // Malte: I had to tweak how these are generated (i.e. using double literals, instead of
            // already converted raw fixed point integer liteals), and I am unsure if it has any
            // significant effect on the precision - I do not think so.
            constexpr TFixed fA { to_fixed(  0.0776509570923569) };
            constexpr TFixed fB { to_fixed(-0.287434475393028) };
            constexpr TFixed fC { to_fixed( 0.995181681698119) }; //PI/4 - A - B

            const TFixed xx = x * x;
            return ((fA*xx + fB)*xx + fC)*x;
        }

        // Calculate atan(y / x), assuming x != 0.
        //
        // If x is very, very small, y/x can easily overflow the fixed-point range.
        // If q = y/x and q > 1, atan(q) would calculate atan(1/q) as intermediate step
        // anyway. We can shortcut that here and avoid the loss of information, thus
        // improving the accuracy of atan(y/x) for very small x.
        template<typename TFixed>
        [[nodiscard]] constexpr TFixed atan_divide(TFixed y, TFixed x)
            noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
        {
            TD_ASSERT(x != 0, "'x' must not be 0");

            // Make sure y and x are positive.
            // If y / x is negative (when y or x, but not both, are negative), negate the result to
            // keep the correct outcome.
            if (y < TFixed(0)) {
                if (x < TFixed(0)) {
                    return atan_divide(-y, -x);
                }
                return -atan_divide(-y, x);
            }
            if (x < TFixed(0)) {
                return -atan_divide(y, -x);
            }
            TD_ASSERT(y >= TFixed(0), "y is negative (was %s) (Internal error)", y);
            TD_ASSERT(x >  TFixed(0), "x is not above 0 (was %s) (Internal error)", x);

            if (y > x) {
                return TFixed::get_half_pi() - internal::atan_sanitized(x / y);
            }
            return internal::atan_sanitized(y / x);
        }
    }

    // Returns radians from number of revolutions (1 is 2 PI)
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed revolutions_to_radians(TFixed num_revolutions) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        // TODO: Test that TFixed can even represent value
        return num_revolutions * TFixed::get_two_pi();
    }

    // Returns radians to "number of revolutions" (i.e. 1 is 360 degrees or 2 PI)
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed radians_to_revolutions(TFixed radians) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        constexpr TFixed PI_MULTIPLY_FACTOR = to_fixed(0.15915494309189500); // 1 / (2 * PI);
        return radians * PI_MULTIPLY_FACTOR;
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed sqrt(TFixed x) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        using TIntermediate = typename TFixed::Type;

        TD_ASSERT(x >= TFixed(0), "Cannot find square root of negative number %s", x);
        
        if (x == TFixed(0)) return x;

        // Finding the square root of an integer in base-2, from:
        // https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

        // Shift by F first because it's fixed-point.
        TIntermediate num = (TIntermediate)(TIntermediate{x.get_raw_value()} << TFixed::NUM_FRACTION_BITS);
        TIntermediate res { 0 };

        // "bit" starts at the greatest power of four that's less than the argument.
        for( TIntermediate bit = (TIntermediate)(TIntermediate{1} << (((TIntermediate)internal::find_highest_bit(static_cast<unsigned long long>(x.get_raw_value())) + TFixed::NUM_FRACTION_BITS) / 2 * 2));
             bit != 0;
            bit >>= 2
        ) {
            const TIntermediate val = res + bit;
            res >>= 1;
            if (num >= val)
            {
                num -= val;
                res += bit;
            }
        }

        // Round the last digit up if necessary
        if (num > res)
        {
            res++;
        }

        return TFixed::from_raw_fixed_value(static_cast<TFixed::Type>(res));
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed abs(TFixed x) noexcept {
        return x < 0 ? -x : x;
    }

    // TODO: This should just be a overloaded operator on TFixed
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed fmod(TFixed x, TFixed y) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        TD_ASSERT(y != 0, "Modulo x by 0 is not valid");
        return TFixed::from_raw_fixed_value(x.get_raw_value() % y.get_raw_value());
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed sin_radians(TFixed x) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {

        // This sine uses a fifth-order curve-fitting approximation originally
        // described by Jasper Vijn on coranac.com which has a worst-case
        // relative error of 0.07% (over [-pi:pi]).
        // Turn x from [0..2*PI] domain into [0..4] domain
        x = fmod(x, TFixed::get_two_pi());
        x = x / TFixed::get_half_pi();

        // Take x modulo one rotation, so [-4..+4].
        if (x < TFixed(0)) {
            x += TFixed(4);
        }

        typename TFixed::Type sign = +1;
        if (x > TFixed(2)) {
            // Reduce domain to [0..2].
            sign = -1;
            x -= TFixed(2);
        }

        if (x > TFixed(1)) {
            // Reduce domain to [0..1].
            x = TFixed(2) - x;
        }

        const TFixed x2 = x * x;
        return  x * sign * (TFixed::get_pi() - x2 * (TFixed::get_two_pi() - 5 - x2 * (TFixed::get_pi() - 3))) / 2;
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed sin(TFixed num_revolutions) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        return sin_radians(revolutions_to_radians(num_revolutions));
    }

    template <typename TFixed>
    constexpr inline TFixed cos_radians(TFixed radians) noexcept
        requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {        
        if( radians == 0 ) {
            return 1;
        }
        else if (radians > 0) {  // Prevent an overflow due to the addition of π/2
            return sin_radians(radians - (TFixed::get_two_pi() - TFixed::get_half_pi()));
        } else {
            return sin_radians(TFixed::get_half_pi() + radians);
        }    
    }

    template <typename TFixed>
    constexpr inline TFixed cos(TFixed num_revolutions) noexcept
        requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        return cos_radians(revolutions_to_radians(num_revolutions));  
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed asin_radians(TFixed x) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        TD_ASSERT(x >= TFixed(-1) && x <= TFixed(+1), "x must be between -1 and 1 (was %s)", x);

        const TFixed yy = TFixed(1) - x * x;
        if (yy == TFixed(0)) {
            return x < 0 ? -TFixed::get_half_pi() : TFixed::get_half_pi();
        }
        return internal::atan_divide(x, sqrt(yy));
    }

    template<typename TFixed>
    [[nodiscard]] constexpr TFixed asin(TFixed x) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        return radians_to_revolutions(asin_radians(x));
    }

    // Returns angle in same fixed point format in radians
    // Warning: I am not convinced this is well optimized for speed, and have not tested the number
    // of cycles it take - so use with caution
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed atan2_radians(TFixed y, TFixed x)
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {

        // From https://github.com/MikeLankamp/fpm/blob/master/include/fpm/math.hpp
        // fitted to td::Fixed use 

        // Source license:
        // MIT License

        // Copyright (c) 2019 Mike Lankamp

        // Permission is hereby granted, free of charge, to any person obtaining a copy
        // of this software and associated documentation files (the "Software"), to deal
        // in the Software without restriction, including without limitation the rights
        // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        // copies of the Software, and to permit persons to whom the Software is
        // furnished to do so, subject to the following conditions:

        // The above copyright notice and this permission notice shall be included in all
        // copies or substantial portions of the Software.

        // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        // SOFTWARE.

        if (x == 0)
        {
            TD_ASSERT(y != TFixed(td::to_fixed(0)), "y must not be 0 if x is 0");
            return (y > TFixed(td::to_fixed(0))) ? TFixed::get_half_pi() : -TFixed::get_half_pi();
        }

        auto ret = internal::atan_divide(y, x);

        if (x < TFixed(td::to_fixed(0)))
        {
            return (y >= TFixed(td::to_fixed(0))) ? ret + TFixed::get_pi() : ret - TFixed::get_pi();
        }
        return ret;
    }


    // Returns angle in "revolutions" (1 is 360 degrees or 2 PI)
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed atan2(TFixed y, TFixed x) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        return radians_to_revolutions(atan2_radians(y, x));
    }
}