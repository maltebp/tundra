#pragma once

namespace td {

    class Int3 {
    public:

        constexpr Int3() = default;

        constexpr Int3(int x, int y, int z) : x(x), y(y), z(z) { }

        int x = 0;
        int y = 0;
        int z = 0;
    };

}