#pragma once

#include <vector>

#include "tundra/model/obj/obj-face.hpp"
#include "tundra/model/obj/obj-material.hpp"


namespace td :: ac {
	
	class ObjObjectPart {
	public:

		ObjMaterial* material = nullptr;

		bool is_smooth_shaded = false;

		std::vector<ObjFace> faces;

	};

}
