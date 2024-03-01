#pragma

#include <tundra/core/types.hpp>

namespace td {

	class ModelAsset;

	class ModelDeserializer {
	public:

		ModelAsset* deserialize(const byte* data) const;
		
	};

}