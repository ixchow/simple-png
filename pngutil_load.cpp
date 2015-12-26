#include "pngutil_load.hpp"

#include "load_save_png.hpp"

#include <sstream>
#include <iostream>

bool pngutil_load(std::string name, uint32_t &width, uint32_t &height, std::vector< uint32_t > &data) {
	std::string special = "";
	std::istringstream special_data;
	{ //check for specials
		auto found = name.find(":");
		if (found < name.size()) {
			special = name.substr(0,found);
			special_data.str(name.substr(found+1));
		}
	}
	if (special == "blank") {
		char x;
		if (!((special_data >> width >> x >> height) && x == 'x' && !(special_data >> x))) {
			std::cerr << "Error creating image; expected 'blank:<w>x<h>', got '" << name << "'." << std::endl;
			return false;
		}
		data.resize(width * height, 0);
		return true;
	} else if (special == "flip") {
		if (!load_png(special_data.str(), width, height, data, LowerLeftOrigin)) {
			std::cerr << "Error creating image; expected 'flip:image.png', got '" << name << "'." << std::endl;
			return false;
		}
		return true;
	} else {
		return load_png(name, width, height, data, UpperLeftOrigin);
	}
}
