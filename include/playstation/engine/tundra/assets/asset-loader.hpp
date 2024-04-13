#pragma once

#include <tundra/assets/sound/sound-asset.hpp>
#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

    class VramAllocator;

    class AssetLoader {
    public:

        AssetLoader(VramAllocator& vram_allocator);

        const SoundAsset* load_sound(const td::byte* data);

        const TextureAsset* load_texture(const td::byte* data);

        const ModelAsset* load_model(const td::byte* data, const TextureAsset* texture_to_map_to = nullptr);

    private:

        VramAllocator& vram_allocator;
        
    };

}