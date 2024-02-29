#include "tundra/asset-compilers/model-compiler/obj-parser.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <tuple>

#include "tundra/core/assert.hpp"
#include "tundra/asset-compilers/model-compiler/obj-model.hpp"
#include "tundra/asset-compilers/model-compiler/float3.hpp"
#include "tundra/asset-compilers/model-compiler/assert-input.hpp"

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

		char c = line_stream_without_comments.get();
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

namespace td::ac {

	ObjModel* ObjParser::parse(const std::filesystem::path& obj_file_path) {

		std::ifstream obj_file;
		obj_file.open(obj_file_path);

		ObjModel* model = new ObjModel();

		std::cout << "Content: " << std::endl;

		std::string line;
		
		while (!obj_file.eof()) {
			TD_ASSERT(!obj_file.fail(), "Stream has failed");

			std::vector<std::string> tokens = parse_line(obj_file);

			if( tokens.size() == 0 ) {
				continue;
			}

			if( tokens[0] == "v" ) {
				td::ac::assert_input(tokens.size() == 4, "Invalid vertex format (only %d tokens)", tokens.size());

				try {
					Float3 vertex{
						std::stof(tokens[1]),
						std::stof(tokens[2]),
						std::stof(tokens[3])
					};
					model->vertices.push_back(vertex);
				}
				catch( std::invalid_argument& e ) {
					assert_input(false, "Ill-formated vertex (%s %s %s)", tokens[1].c_str(), tokens[2].c_str(), tokens[3].c_str());
				}
			}

			std::cout << std::endl;
		}

		return model;
	}

}