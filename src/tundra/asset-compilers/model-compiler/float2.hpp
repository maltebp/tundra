#pragma once

namespace td {

    class Float2 {
    public:

        constexpr Float() = default;

        constexpr Float(float x, float y) : x(x), y(y), z(z) { }

        float x = 0;
        float y = 0;

    };

}