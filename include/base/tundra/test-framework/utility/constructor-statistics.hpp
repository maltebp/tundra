#pragma once

#include <tundra/core/types.hpp>

namespace td {

    class ConstructorStatistics {
    public:

        ConstructorStatistics() {
            num_default_constructors_called++;
            num_constructors_called++;
        }

        ConstructorStatistics(const ConstructorStatistics&)
        {
            num_copy_constructors_called++;
            num_constructors_called++;
        }

        ConstructorStatistics(ConstructorStatistics&&)
        {
            num_constructors_called++;
            num_move_constructors_called++;
        }

        ~ConstructorStatistics() {
            num_destructors_called++;
        }

        static void reset_constructor_counters() {
            num_default_constructors_called = 0;
            num_copy_constructors_called = 0;
            num_constructors_called = 0;
            num_move_constructors_called = 0;
            num_destructors_called = 0;
        }

        static inline td::uint32 num_default_constructors_called = 0;
        static inline td::uint32 num_copy_constructors_called = 0;
        static inline td::uint32 num_constructors_called = 0;
        static inline td::uint32 num_move_constructors_called = 0;
        static inline td::uint32 num_destructors_called = 0;
    };

}