#pragma once

#include <vector>

#include "tundra/asset-compilers/model-compiler/float3.hpp"


namespace td {
    
    class ObjObject {
    public:
        std::vector<int> vertex_indices;
        std::vector<int> normal_indices;
    };

}