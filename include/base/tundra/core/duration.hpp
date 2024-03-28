#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>

namespace td {

    class Duration {
    public:

        constexpr Duration() = default;

        constexpr Duration(int64 microseconds) : microseconds(microseconds) { }

        [[nodiscard]] constexpr Duration operator+(const Duration& other) const {
            return Duration(microseconds + other.microseconds);
        }

        constexpr Duration& operator+=(const Duration& other) {
            microseconds += other.microseconds;
            return *this;
        }

        [[nodiscard]] constexpr Duration operator-(const Duration& other) const {
            return Duration(microseconds - other.microseconds);
        }

        constexpr Duration& operator-=(const Duration& other) {
            microseconds -= other.microseconds;
            return *this;
        }

        constexpr bool operator<(const Duration& other) const {
            return microseconds < other.microseconds;
        }

        constexpr bool operator>(const Duration& other) const {
            return microseconds > other.microseconds;
        }

        [[nodiscard]] constexpr Fixed32<12> to_milliseconds() const { 
            constexpr int32 MAX_FIXED_INTEGER = 524287; // TODO: This should be implemented in Fixed class
            constexpr int64 MAX_MICROSECONDS = MAX_FIXED_INTEGER * 1000LL;

            // TODO: This only looks at the integer part, and not the fraction part
            TD_ASSERT(
                microseconds <= MAX_MICROSECONDS,
                "Duration overflow when trying to cast to milliseconds (%d microseconds is above the max %d representable milliseconds)", MAX_MICROSECONDS, MAX_FIXED_INTEGER);

            return Fixed32<12>::from_raw_fixed_value((int32)((microseconds << 12) / 1000LL));
        }

        [[nodiscard]] constexpr Fixed32<12> to_seconds() const { 
            constexpr int32 MAX_FIXED_INTEGER = 524287; // TODO: This should be implemented in Fixed class
            constexpr int64 MAX_MICROSECONDS = MAX_FIXED_INTEGER * 1000000LL;

            // TODO: This only looks at the integer part, and not the fraction part
            TD_ASSERT(
                microseconds <= MAX_MICROSECONDS,
                "Duration overflow when trying to cast to seconds (%d microseconds is above the max %d representable seconds)", MAX_MICROSECONDS, MAX_FIXED_INTEGER);

            return Fixed32<12>::from_raw_fixed_value((int32)((microseconds << 12) / 1000000LL));
        }

        int64 microseconds = 0;

    };

}