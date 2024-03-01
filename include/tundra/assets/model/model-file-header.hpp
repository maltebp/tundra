#pragma once

#include <tundra/core/types.hpp>

namespace td {
	
	class ModelFileHeader {
	public:

		bool is_valid() const {
			return type_name[0] == 't'
				&& type_name[1] == 'd'
				&& type_name[2] == '_'
				&& type_name[3] == 'm'
				&& type_name[4] == 'o'
				&& type_name[5] == 'd'
				&& type_name[6] == 'e'
				&& type_name[7] == 'l';
		}

		// td_model
		char type_name[8];

		// uint32 size;
		uint16 num_vertices;
		uint16 num_normals;
		uint16 num_parts;
	};

}