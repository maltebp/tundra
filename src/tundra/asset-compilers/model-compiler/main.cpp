#include <iostream>
#include <string> 
#include <filesystem>
#include <stdexcept>

#include "tundra/asset-compilers/model-compiler/obj-model.hpp"

namespace {

    struct Arguments {
        std::filesystem::path input_file_path;
    };

    Arguments parse_args(int argc, const char* argv[]) {
        if( argc != 2 ) {
            throw std::invalid_argument("Invalid number of arguments");    
        }
        
        std::filesystem::path input_file_path { argv[1] };
        if( !std::filesystem::exists(input_file_path) ) {
            throw std::invalid_argument(std::string("Path does not exist: ") + input_file_path.string());
        }   

        return { input_file_path };
    }

    td::ObjModel parse_obj(std::filesystem::path& obj_file_path) {
        std::cout << "Parsing .obj " << obj_file_path << " ... " << std::endl;
        return { };
    }

}

int main(int argc, const char* argv[]) try {

    Arguments parsed_args = parse_args(argc, argv);

    parse_obj(parsed_args.input_file_path);

    return 0;

} catch(std::exception e) {

    std::cout << "Error: " << e.what() << std::endl;

}