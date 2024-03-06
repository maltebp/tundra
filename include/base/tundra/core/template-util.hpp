#pragma once

namespace td {

    template<typename T>
    class DependentFalseType {
    public:
        static constexpr bool value = false;   
    };

}