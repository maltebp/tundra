#pragma once

#include <cstdint>
#include <stdio.h>

#include <tundra/core/string.hpp>

#include <tundra/core/assert.hpp>

namespace td {

    namespace string_util {

        template<typename ... TArgs>
        static String create_from_format(const char* format, const TArgs&... format_args) {
            
            char dummy_buffy;

            int32 num_chars_to_write = snprintf(&dummy_buffy, 1, format, format_args...);
            TD_ASSERT(num_chars_to_write >= 0, "snprintf failed (used format '%s' and it returned %d)", format, num_chars_to_write);

            char* buffer = new char[(uint32)num_chars_to_write + 1];
            int32 num_written_characters  = snprintf(buffer, (uint32)num_chars_to_write + 1, format, format_args...);
            TD_ASSERT(num_written_characters >= 0, "snprintf failed (used format '%s' and it returned %d)", format, num_written_characters);
            TD_ASSERT(num_written_characters == num_chars_to_write, "snprintf did not write the full string (wrote '%d' characters, but should write %d)", format, num_written_characters);

            return String{ buffer, (uint32)num_written_characters };
        }
        
    }

}