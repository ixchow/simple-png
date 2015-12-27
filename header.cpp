#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <vector>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage:\n"
			"\theader <file> <name>\n"
			" Create (and write to stdout) a C++ source file that contains the contents of 'file' as a data string called 'name'."
		<< std::endl;
		return 1;
	}

	std::string name = argv[2];

	std::ifstream file(argv[1], std::ios::binary);
	if (!file) {
		std::cerr << "Error opening '" << argv[1] << "'." << std::endl;
	}

	std::vector< uint8_t > data;
	{
		char temp;
		while (file.get(temp)) {
			data.emplace_back(temp);
		}
	}

	std::string as_string = "";

	as_string += "const size_t " + name + "_length = " + std::to_string(data.size()) + ";\n";
	as_string += "const char " + name + "[" + std::to_string(data.size()) + "] = \n";

	const uint32_t BlockSize = 80;
	for (uint32_t i = 0; i < data.size(); i += BlockSize) {
		std::string s = std::string(data.begin() + i, data.begin() + std::min< uint32_t >(i + BlockSize, data.size()));
		std::string delim = "";
		uint32_t val = 0;
		while (s.find(")" + delim) != std::string::npos) {
			delim = std::to_string(val);
			++val;
		}
		as_string += "R\"" + delim + "(" + s + ")" + delim + "\"\n";
	}

	as_string += ";\n";

	std::cout << as_string;

	return 0;
}
