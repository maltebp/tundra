#pragma once

#include <stdio.h>

#include <tundra/core/assert.hpp>
#include <tundra/core/string.hpp>

namespace td {

    namespace string_util {

        namespace internal {
            extern const uint32 CREATE_FROM_FORMAT_BUFFER_SIZE;
            extern char create_from_format_buffer[];
        }

        template<typename ... TArgs>
        static String create_from_format(const char* format, const TArgs&... format_args) {

            int32 num_written_characters  = snprintf(
                internal::create_from_format_buffer,
                 internal::CREATE_FROM_FORMAT_BUFFER_SIZE, 
                 format, 
                 format_args...);
            TD_ASSERT(num_written_characters >= 0, "snprintf failed (used format '%s' and it returned %d)", format, num_written_characters);

            // snprintf is bugged (or more specifically, vsnprintf that snprintf calls) - no matter
            // the size of the input buffer, it will write the termination char at the address where
            // the output string would have ended had it the buffer been big enough.
            // See: https://github.com/Lameguy64/PSn00bSDK/issues/84
            TD_ASSERT(
                (uint32)num_written_characters < internal::CREATE_FROM_FORMAT_BUFFER_SIZE,
                "MEMORY CORRUPTED! snprintf wrote %d characters when buffer is only %d bytes");

            // Copy buffer
            char* designated_buffer = new char[(uint32)num_written_characters];
            for( uint32 i = 0; i < (uint32)num_written_characters + 1; i++ ) {
                designated_buffer[i] = internal::create_from_format_buffer[i];
            }

            return String{ designated_buffer, (uint32)num_written_characters };
        }
        
    }

}