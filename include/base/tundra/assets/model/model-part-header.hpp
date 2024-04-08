#pragma once

#include <tundra/core/types.hpp>

namespace td {

	class ModelFilePartHeader {
	public: 
		
		bool is_smooth_shaded;

		uint16 num_triangles;

	};
}