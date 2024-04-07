#pragma once

#include <vector>

#include "tundra/model/obj/obj-face.hpp"


namespace td :: ac {
	
	class ObjObjectPart {
	public:

		// TODO: Add material


		std::string material_name;

		bool is_smooth_shaded = false;

		std::vector<ObjFace> faces;

	};

}
