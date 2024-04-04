#pragma once

#include <type_traits>

#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>

namespace td {

    namespace internal {

        // Calculates atan(x) assuming that x is in the range [0,1]
        template<typename TFixed>
        [[nodiscard]] constexpr TFixed atan_sanitized(TFixed x) 
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

            TD_ASSERT(x != TFixed(0), "'x' must not be 0");

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

    // Returns radians to "number of revolutions" (i.e. 1 is 360 degrees or 2 PI)
    template<typename TFixed>
    [[nodiscard]] constexpr TFixed radians_to_revolutions(TFixed radians) 
        noexcept requires(std::is_base_of<internal::FixedNonTemplateBase, TFixed>::value) 
    {
        constexpr TFixed PI_MULTIPLY_FACTOR = to_fixed(0.15915494309189500); // 1 / (2 * PI);
        return radians * PI_MULTIPLY_FACTOR;
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