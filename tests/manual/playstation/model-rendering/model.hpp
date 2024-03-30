#pragma once

#include <psxgte.h>

namespace td {
    class ModelAsset;
}

class TriangleMesh;

class Model {
public: 

    Model(const td::ModelAsset& asset) 
        :   asset(asset)
    { }

    const td::ModelAsset& asset;

    VECTOR position { 0 };

    SVECTOR rotation { 0 };

    SVECTOR scale { ONE, ONE, ONE };

    CVECTOR color { 255, 255, 255 };

};