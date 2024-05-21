#pragma once

#include <climits>

namespace td {

    namespace limits {

        template<typename T>
        struct numeric_limits;

        template<>
        struct numeric_limits<char> {
            static constexpr inline char min = CHAR_MIN;
            static constexpr inline char max = CHAR_MAX;
        };

        template<>
        struct numeric_limits<short> {
            static constexpr inline short min = SHRT_MIN;
            static constexpr inline short max = SHRT_MAX;
        };

        template<>
        struct numeric_limits<int> {
            static constexpr inline int min = INT_MIN;
            static constexpr inline int max = INT_MAX;
        };

        template<>
        struct numeric_limits<long> {
            static constexpr inline long min = LONG_MIN;
            static constexpr inline long max = LONG_MAX;
        };

        template<>
        struct numeric_limits<long long> {
            static constexpr inline long long min = LLONG_MIN;
            static constexpr inline long long max = LLONG_MAX;
        };

        template<>
        struct numeric_limits<unsigned char> {
            static constexpr inline unsigned char min = 0;
            static constexpr inline unsigned char max = UCHAR_MAX;
        };
    
        template<>
        struct numeric_limits<unsigned short> {
            static constexpr inline unsigned short min = 0;
            static constexpr inline unsigned short max = USHRT_MAX;
        };

        template<>
        struct numeric_limits<unsigned int> {
            static constexpr inline unsigned int min = 0;
            static constexpr inline unsigned int max = UINT_MAX;
        };

        template<>
        struct numeric_limits<unsigned long> {
            static constexpr inline unsigned long min = 0;
            static constexpr inline unsigned long max = ULONG_MAX;
        };

        template<>
        struct numeric_limits<unsigned long long> {
            static constexpr inline unsigned long long min = 0;
            static constexpr inline unsigned long long max = ULLONG_MAX;
        };

    }

}