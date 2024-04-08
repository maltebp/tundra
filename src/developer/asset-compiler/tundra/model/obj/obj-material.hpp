#pragma once

#include <string>
#include <filesystem>

#include <tundra/float3.hpp>

namespace td::ac {

    class ObjMaterial {
    public:
        std::string name;
        std::filesystem::path diffuse_texture_path;
        Float3 diffuse_color;

        [[nodiscard]] bool has_diffuse_texture() const {
            return !diffuse_texture_path.empty();
        }
    };

}