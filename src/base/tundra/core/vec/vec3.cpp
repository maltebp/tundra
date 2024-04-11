#include <tundra/core/vec/vec3.hpp>

namespace td {
    
    template<>
    String to_string<char>(const Vec3<char>& v) {
        return string_util::create_from_format(
            "(%s, %s, %s)", 
            to_string((int32)v.x).get_c_string(),
            to_string((int32)v.y).get_c_string(),
            to_string((int32)v.z).get_c_string()
        );
    }

    template<>
    String to_string<unsigned char>(const Vec3<unsigned char>& v) {
        return string_util::create_from_format(
            "(%s, %s, %s)", 
            to_string((uint32)v.x).get_c_string(),
            to_string((uint32)v.y).get_c_string(),
            to_string((uint32)v.z).get_c_string()
        );
    }
}