#pragma once

namespace td {

    class Float3 {
    public:

        constexpr Float3() = default;

        constexpr Float3(float x, float y, float z) : x(x), y(y), z(z) { }

        float x = 0;
        float y = 0;
        float z = 0;

    };

}