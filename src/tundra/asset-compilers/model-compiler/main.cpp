#include <iostream>
#include <string> 
#include <filesystem>
#include <stdexcept>

#include "tundra/asset-compilers/model-compiler/assert-input.hpp"
#include "tundra/asset-compilers/model-compiler/obj-model.hpp"
#include "tundra/asset-compilers/model-compiler/obj-parser.hpp"

namespace {

    struct Arguments {
        std::filesystem::path input_file_path;
    };

    Arguments parse_args(int argc, const char* argv[]) {
        td::ac::assert_input(argc == 2, "Invalid number of arguments (was %d)", argc);

        std::filesystem::path input_file_path { argv[1] };
        td::ac::assert_input(std::filesystem::exists(input_file_path), "Path does not exit: '%s'", input_file_path.string().c_str());

        return { input_file_path };
    }
}

int main(int argc, const char* argv[]) try {

    Arguments parsed_args = parse_args(argc, argv);

    td::ac::ObjParser obj_parser;
    td::ac::ObjModel* obj_model = obj_parser.parse(parsed_args.input_file_path);

    std::cout << std::endl << "Vertices:" << std::endl;
    for( const td::Float3 vertex : obj_model->vertices ) {
        std::cout << '\t' << vertex << std::endl;
    }

    std::cout << std::endl << "Normals:" << std::endl;
    for( const td::Float3 normal : obj_model->normals ) {
        std::cout << '\t' << normal << std::endl;
    }

    std::cout << "Finished parsing model (vertices: " << obj_model->vertices.size() << ", normals: " << obj_model->normals.size() ")" << std::endl;

    return 0;

}
catch (td::ac::InputException e) {
    
    std::cerr << "Error: " << e.what() << std::endl;

} catch(std::exception e) {
    std::cerr << "Internal exception: " << e.what() << std::endl;
    throw e;
}