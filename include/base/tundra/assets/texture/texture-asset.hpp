#pragma once

#include <tundra/core/types.hpp>

namespace td {

    // This is implemented by the platform
    struct TextureAssetLoadInfo;

    enum class TextureMode {
        TrueColor,
        Palette4Bit,
        Palette8Bit
    };
    
    class TextureAsset {
    public:

        TextureMode mode = TextureMode::TrueColor;

        uint8 pixels_width = 0;
        uint8 pixels_height = 0;

        TextureAssetLoadInfo* load_info = nullptr;

    };

}