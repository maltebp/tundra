#pragma once

#include <vector>

#include "tundra/int3.hpp"

namespace td::ac {

	class ObjFace{
	public:

		// Each elements are 3 indices (vertex, texture, normal) or 0 if not set
		std::vector<Int3> indices;

	};

}