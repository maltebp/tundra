#pragma once

#include <tundra/core/types.hpp>
#include <tundra/core/vector.hpp>

namespace td {

	class ModelFilePartHeader {
	public: 
		
		bool is_smooth_shaded;

		uint8 texture_index;

		::Vec3<uint8> color;

		uint16 num_triangles;

	};
}