#pragma once

#include <vector>

#include "tundra/asset-compilers/model-compiler/obj-face.hpp"


namespace td :: ac {
	
	class ObjObjectPart {
	public:

		// TODO: Add material

		std::string material_name;

		std::vector<ObjFace> faces;

	};

}
