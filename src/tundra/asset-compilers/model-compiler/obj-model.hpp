#pragma once

#include <vector>

#include "tundra/asset-compilers/model-compiler/float2.hpp"
#include "tundra/asset-compilers/model-compiler/float3.hpp"

namespace td::ac {

    class ObjModel {
    public:

        std::vector<Float3> vertices;

        std::vector<Float3> normals;

    };

}