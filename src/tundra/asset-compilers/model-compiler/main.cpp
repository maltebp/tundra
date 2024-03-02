#include <iostream>
#include <string> 
#include <filesystem>
#include <stdexcept>
#include <fstream>

#include "tundra/core/types.hpp"
#include "tundra/core/assert.hpp"
#include "tundra/assets/model/model-asset.hpp"
#include "tundra/assets/model/model-part.hpp"
#include "tundra/assets/model/model-deserializer.hpp"

#include "tundra/asset-compilers/model-compiler/assert-input.hpp"
#include "tundra/asset-compilers/model-compiler/obj-model.hpp"
#include "tundra/asset-compilers/model-compiler/obj-object-part.hpp"
#include "tundra/asset-compilers/model-compiler/obj-parser.hpp"
#include "tundra/asset-compilers/model-compiler/model-serializer.hpp"


namespace {

    struct Arguments {
        std::filesystem::path input_file_path;
        std::filesystem::path output_file_path;
    };

    Arguments parse_args(int argc, const char* argv[]) {
        td::ac::input_assert(argc == 3, "Invalid number of arguments (was %d)", argc - 1);

        std::filesystem::path input_file_path { argv[1] };
        td::ac::input_assert(std::filesystem::exists(input_file_path), "Input path does not exit: '%s'", input_file_path.string().c_str());

        std::filesystem::path output_file_path{ argv[2] };

        return { input_file_path, output_file_path };
    }

    void write_compiled_asset(std::filesystem::path& path, const td::byte* data, td::uint32 length) {
        
        std::cout << std::endl;
        std::cout << "Writing compiled asset to '" << path << "'" << std::endl;
        if( std::filesystem::exists(path) ) {
            std::cout << "  File already exists - deleting it" << std::endl;
            std::filesystem::remove(path);
        }

        std::ofstream output_file{};
        output_file.open(path, std::ios::binary | std::ios::out);

        TD_ASSERT(output_file.is_open(), "Failed to open output file");

        output_file.write((const char*)data, length);

        std::cout << "Finished writing asset" << std::endl;
    }
}

int main(int argc, const char* argv[]) try {

    Arguments parsed_args = parse_args(argc, argv);

    td::ac::ObjParser obj_parser;
    td::ac::ObjModel* obj_model = obj_parser.parse(parsed_args.input_file_path);

    std::cout << "Content: " << std::endl;

    std::cout << std::endl << "Vertices:" << std::endl;
    for( const td::Float3 vertex : obj_model->vertices ) {
        std::cout << '\t' << vertex << std::endl;
    }

    std::cout << std::endl << "Normals:" << std::endl;
    for( const td::Float3 normal : obj_model->normals ) {
        std::cout << '\t' << normal << std::endl;
    }

    std::cout << "Finished parsing model (vertices: " 
        << obj_model->vertices.size() 
        << ", normals: " << obj_model->normals.size() 
        << ", objects: " << obj_model->obj_objects.size() 
        << ")" << std::endl;

    for( td::ac::ObjObject* object : obj_model->obj_objects ) {
        std::cout << "\tObject: " << object->name << " (parts: " << object->parts.size() << ", total faces: " << object->num_total_faces() << ")" << std::endl;
        for( td::ac::ObjObjectPart* part : object->parts ) {
            std::cout << "\t\tPart: material = " << part->material_name << ", faces = " << part->faces.size() << std::endl;
        }
    }

    std::cout << "Converting to Model Asser..." << std::endl;
    td::ModelAsset* model_asset = obj_model->to_model_asset();

    std::cout << "Finished parsing Model Asset" << std::endl;
    std::cout << "  Vertices: " << model_asset->num_vertices << std::endl;
    std::cout << "  Normals: " << model_asset->num_normals << std::endl;
    std::cout << "  Parts: " << model_asset->num_parts << std::endl;
    for( int i = 0; i < model_asset->num_parts; i++ ) {
        std::cout << "    Triangles: " << model_asset->model_parts[i]->num_triangles << std::endl;
    }

    /*for( int i = 0; i < model_asset->num_vertices; i++ ) {
        Vec3<td::int16> vertex = model_asset->vertices[i];
        std::cout << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;
    }*/

    //for( int i = 0; i < model_asset->num_parts; i++ ) {
    //    for( int j = 0; j < model_asset->model_parts[i]->num_triangles; j++ ) {
    //        Vec3<td::uint16> vertex_indices = model_asset->model_parts[i]->vertex_indices[j];
    //        std::cout << vertex_indices.x << ", " << vertex_indices.y << ", " << vertex_indices.z << std::endl;
    //    }
    //}

    std::cout << "Testing equality operator...";
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