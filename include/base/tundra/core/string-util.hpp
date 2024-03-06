#pragma once

#include <stdio.h>

#include <tundra/core/string.hpp>

#include <tundra/core/assert.hpp>

namespace td {

    namespace string_util {

        template<typename ... TArgs>
        static String create_from_format(const char* format, const TArgs&... format_args) {
             
            uint32 num_chars_to_write = 0;
            
            num_chars_to_write = snprintf(nullptr, 0, format, format_args...);
            TD_ASSERT(num_chars_to_write >= 0, "snprintf failed (used format '%s' and it returned %d)", format, num_chars_to_write);

            char* buffer = new char[num_chars_to_write + 1];
            uint32 written_bytes  = snprintf(buffer, num_chars_to_write + 1, format, format_args...);
            TD_ASSERT(written_bytes >= 0, "snprintf failed (used format '%s' and it returned %d)", format, written_bytes);
            
            return String{ buffer, num_chars_to_write };
        }
        
    }

}