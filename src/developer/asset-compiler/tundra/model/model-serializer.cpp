#include "tundra/model/model-serializer.hpp"

#include <tundra/core/assert.hpp>

#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-part.hpp>
#include <tundra/assets/model/model-file-header.hpp>
#include <tundra/assets/model/model-part-header.hpp>

namespace td::ac {

	struct ByteArray {
	public:

		ByteArray(uint32 size) : data(new byte[size]), size(size) {
			byte* next = data;
			for( uint32 i = 0; i < size; i++ ) {
				*next = 0;
				next++;
			}
		}

		template<typename T>
		void append_bytes(const T& t) {
			T* bytes_as_t = (T*)next_byte;

			TD_ASSERT((byte*)(bytes_as_t + 1) <= data + size, "ByteArray exceeded size");

			*bytes_as_t = t;
			bytes_as_t++;

			next_byte = (byte*)(bytes_as_t);
		}

		template<typename T>
		void append_bytes_array(const T* t, uint32 num_elements) {
			TD_ASSERT(t != nullptr, "Array pointer must not be nullptr");
			for( uint32 i = 0; i < num_elements; i++ ) {
				append_bytes<T>(t[i]);
			}
		}

		byte* data;
		uint32 size;
		byte* next_byte = data;
	};

	std::tuple<byte*, uint32> ModelSerializer::serialize(const ModelAsset* model) {
		TD_ASSERT(model->num_textures != 0 || model->num_uvs == 0, "Model has UVs but no texture");

		ModelFileHeader file_header{};
		file_header.type_name[0] = 't';
		file_header.type_name[1] = 'd';
		file_header.type_name[2] = '_';
		file_header.type_name[3] = 'm';
		file_header.type_name[4] = 'o';
		file_header.type_name[5] = 'd';
		file_header.type_name[6] = 'e';
		file_header.type_name[7] = 'l';

		file_header.num_vertices = model->num_vertices;
		file_header.num_normals = model->num_normals;
		file_header.num_uvs = model->num_uvs;
		file_header.num_textures = model->num_textures;
		file_header.num_parts = model->num_parts;

		uint32 size = 0;
		size += sizeof(file_header);
		size += model->num_vertices * sizeof(::Vec3<int16>);
		size += model->num_normals * sizeof(::Vec3<int16>);
		size += model->num_uvs * sizeof(::Vec2<int16>);

		for( int i = 0; i < model->num_parts; i++ ) {
			const ModelPart* part = model->model_parts[i];
			size += sizeof(ModelFilePartHeader);
			size += part->num_triangles * sizeof(::Vec3<uint16>);
			size += part->num_triangles * sizeof(::Vec3<uint16>);

			if( part->texture_index != 0 ) {
				size += part->num_triangles * sizeof(::Vec3<uint16>);
			}
		}

		ByteArray serialized_data = ByteArray(size);
		serialized_data.append_bytes(file_header);
		serialized_data.append_bytes_array(model->vertices, model->num_vertices);
		serialized_data.append_bytes_array(model->normals, model->num_normals);
		serialized_data.append_bytes_array(model->uvs, model->num_uvs);

		for( int i = 0; i < model->num_parts; i++ ) {
			const ModelPart* part = model->model_parts[i];
			ModelFilePartHeader part_header{};
			part_header.num_triangles = part->num_triangles;
			part_header.texture_index = part->texture_index;
			part_header.is_smooth_shaded = part->is_smooth_shaded;
			part_header.color = part->color;
			serialized_data.append_bytes(part_header);
			serialized_data.append_bytes_array(part->vertex_indices, part_header.num_triangles);
			serialized_data.append_bytes_array(part->normal_indices, part_header.num_triangles);

			if( part->texture_index != 0 ) {
				serialized_data.append_bytes_array(part->uv_indices, part_header.num_triangles);
			}
		}

		return { serialized_data.data, serialized_data.size };
	}

}