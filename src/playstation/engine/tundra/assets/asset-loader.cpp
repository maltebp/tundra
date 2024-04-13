#include <tundra/assets/asset-loader.hpp>

#include <tundra/sound/playstation-sound.hpp>
#include <tundra/assets/model/model-deserializer.hpp>
#include <tundra/assets/texture/texture-loader.hpp>

namespace td {

    AssetLoader::AssetLoader(VramAllocator& vram_allocator)
        :   vram_allocator(vram_allocator)
    { }

    const SoundAsset* AssetLoader::load_sound(const td::byte* data) {
        TD_ASSERT(data != nullptr, "Sound asset data pointer must not be nullptr");
        return &td::playstation_sound::load_sound(data);
    }

    const TextureAsset* AssetLoader::load_texture(const td::byte* data) {
        TD_ASSERT(data != nullptr, "Texture asset data pointer must not be nullptr");
        return td::texture_loader::load_texture(vram_allocator, data);
    }

    const ModelAsset* AssetLoader::load_model(const td::byte* data, const TextureAsset* texture_to_map_to) {
        TD_ASSERT(data != nullptr, "Texture asset data pointer must not be nullptr");
        ModelAsset* model_asset = td::ModelDeserializer().deserialize(data);
        if( texture_to_map_to != nullptr ) {
            model_asset->map_to_texture(texture_to_map_to);
        }
        return model_asset;
    }

}