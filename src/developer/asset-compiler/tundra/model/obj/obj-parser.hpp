#pragma once

namespace std::filesystem {
	class path;
}

namespace td::ac {

	class ObjModel;

	class ObjParser {
	public:

		ObjModel* parse(const std::filesystem::path& obj_file_path);

	};

}