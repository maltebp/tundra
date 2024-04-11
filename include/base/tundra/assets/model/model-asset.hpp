#pragma once

#include "tundra/core/assert.hpp"
#include <tundra/core/types.hpp>
#include <tundra/core/vector.hpp>
#include <tundra/core/vec/vec2.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/list.dec.hpp>
#include <tundra/core/math.hpp>
#include <tundra/assets/model/model-part.hpp>
#include <tundra/assets/texture/texture-asset.hpp>

namespace td {

	class TextureAsset;
	class ModelPart;

	class ModelAsset {
	public:

		~ModelAsset() {
			if( vertices != nullptr ) delete[] vertices;
			if( normals != nullptr ) delete[] normals;
			if( model_parts != nullptr ) {
				for( int i = 0; i < num_parts; i++ ) {
					delete model_parts[i];
				}
			}
		}

		bool operator==(const ModelAsset& other) const {
			if( name != other.name ) return false;

			if( num_vertices != other.num_vertices ) return false;
			if( num_normals != other.num_normals) return false;
			if( num_uvs != other.num_uvs) return false;
			if( num_textures != other.num_textures ) return false;
			if( num_parts != other.num_parts ) return false;

			for( int i = 0; i < num_vertices; i++ ) {
				if( vertices[i] != other.vertices[i] ) return false;
			}

			for( int i = 0; i < num_normals; i++ ) {
				if( normals[i] != other.normals[i] ) return false;
			}

			for( int i = 0; i < num_uvs; i++ ) {
				if( uvs[i] != other.uvs[i] ) return false;
			}

			for( int i = 0; i < num_parts; i++ ) {
				if( *(model_parts[i]) != *(other.model_parts[i]) ) return false;
			}

			return true;
		}

		bool operator!=(const ModelAsset& other) const {
			return !(*this == other);
		}

		td::uint32 get_total_num_triangles() const {
			td::uint32 total_num_triangles = 0;
			for( uint16 i = 0; i < num_parts; i++ )	{
				total_num_triangles += model_parts[i]->num_triangles;
			}
			return total_num_triangles;
		}

		void set_texture(const TextureAsset* texture) {
			TD_ASSERT(texture != nullptr, "Texture must not be nullptr");
			TD_ASSERT(this->num_textures > 0, "ModelAsset does not need any textures");
			TD_ASSERT(this->num_uvs > 0, "ModelAsset has no UVs");
			TD_ASSERT(this->uvs != nullptr, "ModelAsset's UV list is nullptr");
			TD_ASSERT(this->texture == nullptr, "ModelAsset already has is texture set");

			this->texture = texture;
			
			for( uint16 i = 0; i < num_uvs; i++ ) {
				td::Vec2<Fixed16<12>> uv {
					Fixed16<12>::from_raw_fixed_value(uvs[i].x),
					Fixed16<12>::from_raw_fixed_value(uvs[i].y),
				};

				mapped_uvs.add({
					(uint8)td::round(uv.x * texture->pixels_width).get_raw_integer(),
					(uint8)td::round(uv.x * texture->pixels_width).get_raw_integer()
				});
			}
		}

		const char* name = nullptr;

		uint16 num_vertices = 0;
		::Vec3<int16>* vertices = nullptr;

		uint16 num_normals = 0;
		::Vec3<int16>* normals = nullptr;

		uint16 num_uvs = 0;
		::Vec2<int16>* uvs = nullptr;

		uint8 num_textures = 0;

		uint16 num_parts = 0;
		ModelPart** model_parts = nullptr;

		// Not serialized
		const TextureAsset* texture = nullptr;
		List<td::Vec2<uint8>> mapped_uvs;
	};

}