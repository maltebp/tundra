#pragma once

#include <tundra/core/vec/vec3.hpp>

#include <tundra/assets/model/model-asset.hpp>

#include <tundra/engine/dynamic-transform.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/entity-system/component.dec.hpp>

namespace td {

    class Model : public Component<Model> {
    public: 

        Model(const td::ModelAsset& asset, uint32 layer_index, Transform* const transform) 
            :   asset(asset),
                layer_index(layer_index),
                transform(transform)
        { }

        const td::ModelAsset& asset;
        
        uint32 layer_index;

        ComponentRef<Transform> transform;

        Vec3<uint8> color { 255U, 255U, 255U };

    };

}