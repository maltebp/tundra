#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vector.hpp>

namespace td {

	class ModelPart {
	public:

		~ModelPart() {
			if( vertex_indices != nullptr ) delete[] vertex_indices;
			if( normal_indices != nullptr ) delete[] normal_indices;
		}

		bool operator==(const ModelPart& other) const {
			if( num_triangles != other.num_triangles ) return false;
			if( is_smooth_shaded != other.is_smooth_shaded ) return false;
			
			for( int i = 0; i < num_triangles; i++ ) {
				if( vertex_indices[i] != other.vertex_indices[i] ) return false;
				if( normal_indices[i] != other.normal_indices[i] ) return false;
			}

			return true;
		}

		bool operator!=(const ModelPart& other) const {
			return !(*this == other);
		}

		uint16 num_triangles;

		::Vec3<uint16>* vertex_indices;
		::Vec3<uint16>* normal_indices;

		bool is_smooth_shaded = false;
	};

}