#include <tundra/playstation-time.hpp>

#include <hwregs_c.h>

// Disabling warning for anonymous struct in TCB (not using it, so it shouldn't be a problem)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <psxapi.h>
#pragma GCC diagnostic pop

#include <psxetc.h>

#include <tundra/core/fixed.hpp>
#include <tundra/core/assert.hpp>

namespace td {

    namespace internal::time {

        constexpr int32 TIMER_2_ID = 2;

        constexpr int32 NUM_WHOLE_MICRO_SECONDS_PER_FILL = 15000;
        constexpr int32 NUM_REMAINDER_CYCLES_PER_FILL = 16256;

        // Smallest whole number of cycles that represent a whole number of micro seconds
        constexpr int32 SMALLEST_MICROSECONDS_CYCLES = 21168;

        // Smallest whole micro seconds that can be represented by a whole number of cycles
        constexpr int32 SMALLEST_MICROSECONDS = 625;

        bool is_initialized = false;

        volatile uint32 microseconds_since_init = 0;

        volatile uint32 remainder_cycles = 0;

        void timer_tick() {
            // We use the remainder to avoid having to do potentially expensive division

            microseconds_since_init = microseconds_since_init + NUM_WHOLE_MICRO_SECONDS_PER_FILL;
            remainder_cycles = remainder_cycles + NUM_REMAINDER_CYCLES_PER_FILL;
            if( remainder_cycles > SMALLEST_MICROSECONDS_CYCLES ) {
                remainder_cycles = remainder_cycles - SMALLEST_MICROSECONDS_CYCLES;
                microseconds_since_init = microseconds_since_init + SMALLEST_MICROSECONDS;
            }
        }

        void initialize() {
            TD_ASSERT(!is_initialized, "Time system is already initialized");

            // TODO: Check that ResetGraph is called first
            // WARNING: THIS REQUIRES ResetGraph to be called first

            is_initialized = true;

            uint16 timer_2_settings = 0;

            // IRQ trigger when full: enabled
            timer_2_settings |= 0b0000'0000'0010'0000;

            // IRQ repeat mode: enabled
            timer_2_settings |= 0b0000'0000'0100'0000;
            
            // Increment mode: every 8th system clock
            timer_2_settings |= 2 << 8;

            EnterCriticalSection();

            TIMER_CTRL(TIMER_2_ID) = timer_2_settings;

            // Configure timer 2 IRQ
            ChangeClearRCnt(TIMER_2_ID, 0);
            InterruptCallback(IRQ_Channel::IRQ_TIMER2, &timer_tick);
            ExitCriticalSection();
        }  

        uint64 get_micro_seconds_since_start() {

            // Max value is 545456 cycles, which uses at most 20-bits, so we can "safely"
            // bit-shift it by 12, divide it by a fixed point number of 12 fraction bits
            // and then get out the whole number of times it was divisible with no full
            // precision
            volatile uint32 non_accumulated_cycles = ((uint32)(TIMER_VALUE(TIMER_2_ID) & 0x0FFFF) + remainder_cycles) * 8;
            const UFixed32<12> CYCLES_PER_MICROSECOND { td::to_fixed(33.8688) };
            uint32 non_accumulated_micro_seconds = (non_accumulated_cycles << 12) / CYCLES_PER_MICROSECOND.get_raw_value();

            uint64 total_micro_seconds = microseconds_since_init + non_accumulated_micro_seconds;
            return total_micro_seconds;         
        }

    }   

    PlayStationTime::PlayStationTime()
        :   initialization_time_microsecond(internal::time::microseconds_since_init)
    {
        if( !internal::time::is_initialized ) {
            internal::time::initialize();
        }
    }

    Duration PlayStationTime::get_duration_since_start() const {
        return Duration{(int64)(internal::time::get_micro_seconds_since_start() - initialization_time_microsecond)};
    }
}