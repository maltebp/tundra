#include <iostream>
#include <string> 
#include <filesystem>
#include <stdexcept>
#include <fstream>

#include <tundra/core/types.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.hpp>
#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-part.hpp>
#include <tundra/assets/model/model-deserializer.hpp>

#include "tundra/assert-input.hpp"
#include "tundra/model/model-serializer.hpp"
#include "tundra/model/obj/obj-model.hpp"
#include "tundra/model/obj/obj-object-part.hpp"
#include "tundra/model/obj/obj-parser.hpp"



namespace {

    struct Arguments {
        std::filesystem::path input_file_path;
        std::filesystem::path output_file_path;
    };

    Arguments parse_args(int argc, const char* argv[]) {
        int actual_argc = argc - 1;
        td::ac::input_assert(actual_argc >= 1 && actual_argc <= 2, "Expected %d to %d argumentes (was %d)", 1, 2, actual_argc);

        std::filesystem::path input_file_path { argv[1] };
        
        td::ac::input_assert(std::filesystem::exists(input_file_path), "Input path does not exit: '%s'", input_file_path.string().c_str());

        std::filesystem::path output_file_path;
        if( actual_argc == 2 ) {
            output_file_path = { argv[2] };
        }
        else {
            output_file_path = input_file_path;
            output_file_path.replace_extension("td_model");
        }
        
        return { input_file_path, output_file_path };
    }

    void write_compiled_asset(std::filesystem::path& path, const td::byte* data, td::uint32 length) {
        
        std::cout << std::endl;
        std::cout << "Writing compiled asset to '" << path << "'" << std::endl;
        if( std::filesystem::exists(path) ) {
            std::cout << "  File already exists - deleting it" << std::endl;
            std::filesystem::remove(path);
        }

        std::filesystem::create_directories(path.parent_path());

        std::ofstream output_file{};
        output_file.open(path, std::ios::binary | std::ios::out);

        TD_ASSERT(output_file.is_open(), "Failed to open output file");

        output_file.write((const char*)data, length);

        std::cout << "Finished writing asset" << std::endl;
    }
}


template<typename T>
static inline std::ostream& operator<<(std::ostream& stream, const ::Vec3<T> f3) {
    return stream << "(" << f3.x << ", " << f3.y << ", " << f3.z << ")";
}

template<>
static inline std::ostream& operator<< <td::uint8>(std::ostream& stream, const ::Vec3<td::uint8> f3) {
    return stream << "(" << (td::uint32)f3.x << ", " << (td::uint32)f3.y << ", " << (td::uint32)f3.z << ")";
}


int main(int argc, const char* argv[]) try {

    Arguments parsed_args = parse_args(argc, argv);

    std::cout << "Compiling asset: " << parsed_args.input_file_path << std::endl;

    td::ac::ObjParser obj_parser;
    td::ac::ObjModel* obj_model = obj_parser.parse(parsed_args.input_file_path);

    /*std::cout << "Content: " << std::endl;
    std::cout << std::endl << "Vertices:" << std::endl;
    for( const td::Float3 vertex : obj_model->vertices ) {
        std::cout << '\t' << vertex << std::endl;
    }

    std::cout << std::endl << "Normals:" << std::endl;
    for( const td::Float3 normal : obj_model->normals ) {
        std::cout << '\t' << normal << std::endl;
    }*/
    
    std::cout << "Finished parsing model (vertices: "
        << obj_model->vertices.size()
        << ", normals: " << obj_model->normals.size()
        << ", uvs: " << obj_model->uvs.size()
        << ", objects: " << obj_model->obj_objects.size()
        << ", materials: " << obj_model->materials.size()
        << ")" << std::endl;

    std::cout << "  Materials:" << std::endl;
    for( td::ac::ObjMaterial* material: obj_model->materials ) {
        std::cout << "    " << material->name << ": color = " << material->diffuse_color << ", texture = " << material->diffuse_texture_path << std::endl;
    }

    std::cout << "  Objects:" << std::endl;
    for( td::ac::ObjObject* object : obj_model->obj_objects ) {
        std::cout << "    " << object->name << " (parts: " << object->parts.size() << ", total faces: " << object->num_total_faces() << ")" << std::endl;
        for( td::ac::ObjObjectPart* part : object->parts ) {
            std::cout << "      Part: material = " << (part->material == nullptr ? "none" : part->material->name) << ", faces = " << part->faces.size() << ", smooth = " << (part->is_smooth_shaded ? "true" : "false") << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "Converting to Model Asset..." << std::endl;
    td::ModelAsset* model_asset = obj_model->to_model_asset();
    std::cout << "Finished parsing Model Asset" << std::endl;
    std::cout << "  Vertices: " << model_asset->num_vertices << std::endl;
    std::cout << "  Normals: " << model_asset->num_normals << std::endl;
    std::cout << "  UVs: " << model_asset->num_uvs << std::endl;
    std::cout << "  Textures: " << (td::uint32)model_asset->num_textures << std::endl;
    std::cout << "  Parts: " << model_asset->num_parts << std::endl;

    for( int i = 0; i < model_asset->num_parts; i++ ) {
        td::ModelPart* part = model_asset->model_parts[i];
        std::cout << "    Triangles: " << part->num_triangles << ", Color = " << part->color << ", Texture = " << (td::uint32)part->texture_index << ", Smooth = " << part->is_smooth_shaded << std::endl;
    }

    /*std::cout << "Vertices: " << std::endl;
    for( int i = 0; i < model_asset->num_vertices; i++ ) {
        Vec3<td::int16> vertex = model_asset->vertices[i];
        td::Vec3<td::Fixed16<12>> v{ td::Fixed16<12>::from_raw_fixed_value(vertex.x), td::Fixed16<12>::from_raw_fixed_value(vertex.y), td::Fixed16<12>::from_raw_fixed_value(vertex.z) };
        std::cout << td::to_string(v).get_c_string() << std::endl;
    }

    std::cout << "Indices: " << std::endl;
    for( int i = 0; i < model_asset->num_parts; i++ ) {
        for( int j = 0; j < model_asset->model_parts[i]->num_triangles; j++ ) {
            Vec3<td::uint16> vertex_indices = model_asset->model_parts[i]->vertex_indices[j];
            std::cout << "  " << vertex_indices.x << ", " << vertex_indices.y << ", " << vertex_indices.z << std::endl;
        }
    }*/
       
    std::cout << std::endl;
    std::cout << "Validating...";
    TD_ASSERT((*model_asset) == (*model_asset), "ModelAsset operator== failed (object is not equal to itself)");
    std::cout << "Done";

    std::cout << std::endl;
    std::cout << "Serializing Model Asset..." << std::endl;
    td::ac::ModelSerializer model_serializer;
    auto [data_byte_ptr, num_bytes] = model_serializer.serialize(model_asset);
    std::cout << "Finished serializing Model Asset" << std::endl;
    std::cout << "  Bytes: " << num_bytes << std::endl;

    std::cout << std::endl;
    std::cout << "Deserializing ModelAsset..." << std::endl;
    td::ModelDeserializer model_deserializer;
    td::ModelAsset* deserialized_model_asset = model_deserializer.deserialize(data_byte_ptr);
    std::cout << "Finished deserializing Model Asset" << std::endl;

    std::cout << "Testing deserialized against serialized..." << std::endl;

    TD_ASSERT((*deserialized_model_asset) == (*model_asset), "Deserialized does not equal to serialized");

    write_compiled_asset(parsed_args.output_file_path, data_byte_ptr, num_bytes);

    std::cout << "Done" << std::endl;

    return 0;

}
catch (td::ac::InputException e) {
    
    std::cerr << "Error: " << e.what() << std::endl;

} catch(std::exception e) {
    std::cerr << "Internal exception: " << e.what() << std::endl;
    throw e;
}