#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vector.hpp>
#include <tundra/assets/model/model-part.hpp>

namespace td {

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
			if( num_parts != other.num_parts ) return false;

			for( int i = 0; i < num_vertices; i++ ) {
				if( vertices[i] != other.vertices[i] ) return false;
			}

			for( int i = 0; i < num_normals; i++ ) {
				if( normals[i] != other.normals[i] ) return false;
			}

			for( int i = 0; i < num_parts; i++ ) {
				if( *(model_parts[i]) != *(other.model_parts[i]) ) return false;
			}

			return true;
		}

		bool operator!=(const ModelAsset& other) const {
			return !(*this == other);
		}

		const char* name = nullptr;

		uint16 num_vertices = 0;
		Vec3<int16>* vertices = nullptr;

		uint16 num_normals = 0;
		Vec3<int16>* normals = nullptr;

		uint16 num_parts = 0;
		ModelPart** model_parts = nullptr;
	};

}