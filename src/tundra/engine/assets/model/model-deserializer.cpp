#include "tundra/assets/model/model-deserializer.hpp"

#include "tundra/core/assert.hpp"
#include "tundra/assets/model/model-asset.hpp"
#include "tundra/assets/model/model-part.hpp"
#include "tundra/assets/model/model-file-header.hpp"
#include "tundra/assets/model/model-part-header.hpp"

namespace td {

	template<typename T>
	T& read_bytes(const byte*& data) {
		T* read_data = (T*)data;
		data = (const byte*)(read_data + 1);
		return *read_data;
	}

	ModelAsset* ModelDeserializer::deserialize(const byte* data) const {
		const byte* next_data = data;
		ModelAsset* model_asset = new ModelAsset();

		ModelFileHeader& file_header = read_bytes<ModelFileHeader>(next_data);
		TD_ASSERT(file_header.is_valid(), "Deserialized ModelFileHeader was not valid");
		
		model_asset->num_vertices = file_header.num_vertices;
		model_asset->num_normals = file_header.num_normals;
		model_asset->num_parts = file_header.num_parts;

		model_asset->vertices = new Vec3<int16>[model_asset->num_vertices];
		model_asset->normals = new Vec3<int16>[model_asset->num_normals];
		model_asset->model_parts = new ModelPart*[model_asset->num_parts];

		for( int i = 0; i < model_asset->num_vertices; i++ ) {
			model_asset->vertices[i] = read_bytes<Vec3<int16>>(next_data);
		}

		for( int i = 0; i < model_asset->num_normals; i++ ) {
			model_asset->normals[i] = read_bytes<Vec3<int16>>(next_data);
		}

		for( int i = 0; i < model_asset->num_parts; i++ ) {

			ModelPart* model_part = new ModelPart();

			ModelFilePartHeader& part_header = read_bytes<ModelFilePartHeader>(next_data);
			model_part->num_triangles = part_header.num_triangles;
			model_part->vertex_indices = new Vec3<uint16>[model_part->num_triangles];
			model_part->normal_indices = new Vec3<uint16>[model_part->num_triangles];

			for( int j = 0; j < part_header.num_triangles; j++ ) {
				model_part->vertex_indices[j] = read_bytes<Vec3<uint16>>(next_data);
			}

			for( int j = 0; j < part_header.num_triangles; j++ ) {
				model_part->normal_indices[j] = read_bytes<Vec3<uint16>>(next_data);
			}

			model_asset->model_parts[i] = model_part;
		}

		return model_asset;
	}

}