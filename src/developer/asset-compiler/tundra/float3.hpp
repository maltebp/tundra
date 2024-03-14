#pragma once

#include <ostream>

namespace td {

    class Float3 {
    public:

        constexpr Float3() = default;

        constexpr Float3(float x, float y, float z) : x(x), y(y), z(z) { }

        float x = 0;
        float y = 0;
        float z = 0;

    };

    static inline std::ostream& operator<<(std::ostream& stream, const Float3 f3) {
        return stream << "(" << f3.x << ", " << f3.y << ", " << f3.z << ")";
    }

}