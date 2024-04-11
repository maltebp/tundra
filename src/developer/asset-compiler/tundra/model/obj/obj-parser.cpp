#include "tundra/model/obj/obj-parser.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <tuple>

#include <tundra/core/assert.hpp>

#include "tundra/float3.hpp"
#include "tundra/int3.hpp"
#include "tundra/assert-input.hpp"
#include "tundra/model/obj/obj-model.hpp"
#include "tundra/model/obj/obj-object.hpp"
#include "tundra/model/obj/obj-object-part.hpp"
#include "tundra/model/obj/obj-material.hpp"


std::vector<std::string> parse_line(std::ifstream& stream) {
	TD_ASSERT(!stream.fail(), "Stream is in fail state");
	TD_ASSERT(!stream.eof(), "Stream has reached end");

	std::vector<std::string> tokens;

	std::string line;
	std::getline(stream, line);
	if( line.empty() ) return tokens;

	std::istringstream line_stream{ line };
	
	std::string line_without_comments;
	std::getline(line_stream, line_without_comments, '#');
	if( line_without_comments.empty() ) return tokens;

	std::istringstream line_stream_without_comments{ line_without_comments };

	std::string token;

	while( true ) {
		if( line_stream_without_comments.eof() ) break;

		char c = (char)line_stream_without_comments.get();
		TD_ASSERT(
			line_stream_without_comments.eof() || !line_stream_without_comments.fail(),
			"Line stream has reached fail state without reaching end-of-file when reading char");

		if( line_stream_without_comments.eof() || std::isspace(c) ) {
			if( !token.empty() ) {
				tokens.push_back(token);
				token.clear();
			}
		}
		else
		{
			token += c;
		}
	}

	return tokens;	
}

td::Int3 parse_index(const std::string& index_token) {
	std::istringstream line_stream{ index_token };
	std::string index_string;
	int vertex_index = -1;
	int normal_index = -1;
	int uv_index = -1;

	int index_count = 0;
	while( std::getline(line_stream, index_string, '/') ) {
		index_count++;
		if( index_string.size() == 0 ) continue;
		if( index_count == 1 ) vertex_index = std::stoi(index_string);
		if( index_count == 2 ) uv_index = std::stoi(index_string);
		if( index_count == 3 ) normal_index = std::stoi(index_string);
	}

	td::ac::input_assert(index_count == 3, "Invalid number of elements in face index (was %d)", index_count);

	return { vertex_index, uv_index, normal_index };
}



namespace td::ac {

	std::vector<ObjMaterial*> parse_material_lib(std::filesystem::path material_lib_path);

	ObjModel* ObjParser::parse(const std::filesystem::path& obj_file_path) {

		std::ifstream obj_file;
		obj_file.open(obj_file_path);

		ObjModel* model = new ObjModel();

		bool current_object_is_default = true;
		ObjObject* current_object = new ObjObject();
		
		bool current_object_part_is_default = false;
		ObjObjectPart* current_object_part = new ObjObjectPart();
		current_object->parts.push_back(current_object_part);

		ObjMaterial* current_material = nullptr;
		bool current_smooth_shading_flag = false;

		std::string line;

		auto start_new_object_part = [&]() mutable {
			bool current_is_unused = current_object_part->faces.size() == 0;
			if( current_object_part_is_default && current_is_unused ) {
				// Re-use the default
				current_object_part->material = current_material;
				current_object_part->is_smooth_shaded = current_smooth_shading_flag;
				current_object_part_is_default = false;
			}
			else
			{
				td::ac::input_assert_warning(current_object_part->faces.size() > 0, "Object part has no faces");
				current_object_part = new ObjObjectPart();
				current_object_part->material = current_material;
				current_object_part->is_smooth_shaded = current_smooth_shading_flag;
				current_object->parts.push_back(current_object_part);
				current_object_part_is_default = false;
			}
		};
		
		while (!obj_file.eof()) {
			TD_ASSERT(!obj_file.fail(), "Stream has failed");

			std::vector<std::string> tokens = parse_line(obj_file);

			if( tokens.size() == 0 ) {
				continue;
			}

			if( tokens[0] == "mtllib" ) {
				std::filesystem::path material_lib_path = tokens[1];
				if( material_lib_path.is_relative() ) {
					material_lib_path = obj_file_path.parent_path() / material_lib_path;
				}

				td::ac::input_assert(std::filesystem::exists(material_lib_path), "Cannot find material lib '%s'", material_lib_path.string().c_str());
				td::ac::input_assert(!std::filesystem::is_directory(material_lib_path), "Material lib '%s' is a directory, not a file", material_lib_path.string().c_str());

				std::vector<ObjMaterial*> materials_in_lib = parse_material_lib(material_lib_path);
				for( ObjMaterial* material : materials_in_lib ) {
					model->materials.insert(material);
				}
			}

			if( tokens[0] == "g" || tokens[0] == "o" ) {
				std::string name;
				for( int i = 1; i < tokens.size(); i++ ) {
					name += tokens[i];
					if( i < tokens.size() - 1 ) {
						name += " ";
					}
				}

				td::ac::input_assert_warning(!name.empty(), "Object has no name");

				bool is_default_and_is_unused = current_object_is_default && !current_object->has_any_faces();
				if( is_default_and_is_unused ) {
					delete current_object;
				}
				else
				{
					td::ac::input_assert_warning(current_object->has_any_faces(), "Object '%s' has no faces", current_object->name);
					model->obj_objects.push_back(current_object);
				}

				current_object = new ObjObject();
				current_object->name = name;
				current_object_is_default = false;

				current_object_part = new ObjObjectPart();
				current_object->parts.push_back(current_object_part);
				current_object_part_is_default = true;
			}

			if( tokens[0] == "s" ) {
				td::ac::input_assert(tokens.size() == 2, "'s' must be followed by an 0 or 1 (was followed by %d token)", tokens.size() - 1);
				td::ac::input_assert(current_object != nullptr, "'s' used before an object/group has been defined (we currently don't support this)");

				if( tokens[1] == "1" || tokens[1] == "on" ) {
					current_smooth_shading_flag = true;
				}
				else if( tokens[1] == "0" || tokens[1] == "off" ) {
					current_smooth_shading_flag = false;
				}
				else {
					td::ac::input_assert(false, "Invalid argument %d for 's' token (must be 0, 1, on or off)");
				}

				start_new_object_part();
			}

			if( tokens[0] == "usemtl" ) {
				td::ac::input_assert(current_object != nullptr, "'usemtl' used before an object/group has been defined (we currently don't support this)");
				td::ac::input_assert(model->materials.size() > 0, "'usemtl' before any materials has been loaded");

				std::string material_name;
				for( int i = 1; i < tokens.size(); i++ ) {
					material_name += tokens[i];
					if( i < tokens.size() - 1 ) {
						material_name += " ";
					}
				}

				td::ac::input_assert(!material_name.empty(), "'usemtl' has no name");

				auto it = std::find_if(model->materials.begin(), model->materials.end(), [&material_name](ObjMaterial* m) { return m->name == material_name; });
				td::ac::input_assert(it != model->materials.end(), "Found no material named '%s' in material lib", material_name);

				current_material = *it;

				start_new_object_part();
			}

			if( tokens[0] == "v" ) {
				td::ac::input_assert(tokens.size() == 4, "Invalid vertex format (only %d tokens)", tokens.size());

				try {
					Float3 vertex{
						std::stof(tokens[1]),
						std::stof(tokens[2]),
						std::stof(tokens[3])
					};
					model->vertices.push_back(vertex);
				}
				catch( std::invalid_argument&) {
					input_assert(false, "Ill-formated vertex (%s %s %s)", tokens[1].c_str(), tokens[2].c_str(), tokens[3].c_str());
				}
			}

			if( tokens[0] == "vn" ) {
				td::ac::input_assert(tokens.size() == 4, "Invalid normal format (only %d tokens)", tokens.size());

				try {
					Float3 normal{
						std::stof(tokens[1]),
						std::stof(tokens[2]),
						std::stof(tokens[3])
					};
					model->normals.push_back(normal);
				}
				catch( std::invalid_argument&) {
					input_assert(false, "Ill-formated normal (%s %s %s)", tokens[1].c_str(), tokens[2].c_str(), tokens[3].c_str());
				}
			}

			if( tokens[0] == "vt" ) {
				td::ac::input_assert(tokens.size() == 3, "Invalid 'vt' format (expected 3 tokens, but there was %d)", tokens.size());

				try {
					Float2 uv {
						std::stof(tokens[1]),
						std::stof(tokens[2])
					};
					model->uvs.push_back(uv);
				}
				catch( std::invalid_argument& ) {
					input_assert(false, "Ill-formatted uv coordinate (%s %s)", tokens[1].c_str(), tokens[2].c_str());
				}
			}

			if( tokens[0] == "f" ) {
				td::ac::input_assert(tokens.size() > 3, "Invalid index format (%d tokens)", tokens.size());

				ObjFace face;

				for( int i = 1; i < tokens.size(); i++ ) {
                    Int3 index = parse_index(tokens[i]);
					face.indices.push_back(index);
				}   

				current_object_part->faces.push_back(face);
			}

			if( tokens[0] == "l" ) {
				td::ac::input_assert(false, "lines elements ('l' token) are not supported");
			}
		}

		td::ac::input_assert_warning(current_object_part_is_default || current_object->has_any_faces(), "Last object '%s' has no faces", current_object->name);

		if( !current_object_is_default || current_object->has_any_faces() ) {
			model->obj_objects.push_back(current_object);
		}

		return model;
	}

	std::vector<ObjMaterial*> parse_material_lib(std::filesystem::path material_lib_path) {
		std::ifstream material_lib_file;
		material_lib_file.open(material_lib_path);

		std::vector<ObjMaterial*> materials;

		ObjMaterial* current_material = nullptr;
		std::string line;
		

		while( !material_lib_file.eof() ) {
			TD_ASSERT(!material_lib_file.fail(), "Filestream failed when reading material lib '%s'", material_lib_path);
			
			std::vector<std::string> tokens = parse_line(material_lib_file);

			if( tokens.size() == 0 ) continue;

			if( tokens[0] == "newmtl" ) {

				td::ac::input_assert(tokens.size() > 1, "'newmtl' was not followed by a name");
				td::ac::input_assert(tokens.size() == 2, "'newmtl' has too many arguments (names with spaces is currently not supported)");

				current_material = new ObjMaterial();
				current_material->name = tokens[1];

				materials.push_back(current_material);
			}

			if( tokens[0] == "Kd" ) {
				td::ac::input_assert(tokens.size() == 4, "'Kd' has invalid number of arguments (expecting 3 floats, had %d)", tokens.size() - 1);
				td::ac::input_assert(current_material != nullptr, "'Kd' used before 'newmtl'");

				try {
					current_material->diffuse_color.x = std::stof(tokens[1]);
					current_material->diffuse_color.y = std::stof(tokens[2]);
					current_material->diffuse_color.z = std::stof(tokens[3]);
				}
				catch( std::invalid_argument& ) {
					td::ac::input_assert(false, "Failed to parse Kd value '%s %s %s' as a float", tokens[1], tokens[2], tokens[3]);
				}
			}

			if( tokens[0] == "map_Kd" ) {
				td::ac::input_assert(tokens.size() > 1, "'map_Kd' was not followed by a path");
				td::ac::input_assert(tokens.size() == 2, "'map_Kd' has too many arguments (paths with spaces is currently not supported)");
				td::ac::input_assert(current_material != nullptr, "'map_Kd' used before 'newmtl'");

				current_material->diffuse_texture_path = tokens[1];
			}
		}

		int i = 0;
		for( ObjMaterial* material : materials ) {
			bool material_has_no_values = material->diffuse_color == Float3{ 1, 1, 1 } && material->diffuse_texture_path.empty();
			td::ac::input_assert_warning(!material_has_no_values, "Material %d has no texture nor diffuse color", i);
			i++;
		}

		return materials;
	}

}