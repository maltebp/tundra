#pragma once

#include <vector>

#include "tundra/float2.hpp"
#include "tundra/float3.hpp"
#include "tundra/model/obj/obj-object.hpp"

namespace td {
    class ModelAsset;
}

namespace td::ac {

    class ObjModel {
    public:

        std::vector<Float3> vertices;

        std::vector<Float3> normals;

        std::vector<Float2> uvs;

        std::vector<ObjObject*> obj_objects;

        std::set<ObjMaterial*> materials;

        ModelAsset* to_model_asset() const;

    };

}