#pragma once

#include <tuple>

#include <tundra/core/types.hpp>

namespace td {
	class ModelAsset;
}

namespace td::ac {

	class ModelSerializer {
	public:

		// Data pointer and size of data
		std::tuple<byte*, uint32> serialize(const ModelAsset* model);

	};

}
