#pragma once

namespace td {

    class Float2 {
    public:

        constexpr Float2() = default;

        constexpr Float2(float x, float y) : x(x), y(y) { }

        float x = 0;
        float y = 0;

    };

}