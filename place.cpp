#include "load_save_png.hpp"
#include "pngutil_load.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <sstream>

void usage() {
	std::cerr << "Usage:\n"
		"\tplace <base.png> (<over.png> +<x>+<y>)+ <out.png>\n"
		" Place each over.png at x,y atop base.png\n"
		" (Note: origin is in upper left.)\n"
	<< std::endl;
	exit(1);
}

int main(int argc, char **argv) {

	if (argc < 5 || (argc % 2 == 0)) {
		usage();
	}
	
	std::vector< uint32_t > data;
	uint32_t width, height;

	if (!pngutil_load(argv[1], width, height, data)) {
		std::cerr << "Failed to load '" << argv[1] << "'" << std::endl;
		return 1;
	}

	for (int arg = 2; arg + 1 < argc; arg += 2) {
		uint32_t over_x,over_y;

		std::istringstream geom_str(argv[arg+1]);
		char x,plus1,plus2;
		if (!(geom_str >> plus1 >> over_x >> plus2 >> over_y) || plus1 != '+' || plus2 != '+' || (geom_str >> x)) {
			usage();
		}

		std::vector< uint32_t > over;
		uint32_t over_width, over_height;
		if (!pngutil_load(argv[arg], over_width, over_height, over)) {
			std::cerr << "Failed to load '" << argv[arg] << "'" << std::endl;
			return 1;
		}

		if (uint32_t(over_x + over_width) > width || uint32_t(over_y + over_height) > height) {
			std::cerr << "Over image of size " << over_width << "x" << over_height << " at " << over_x << ", " << over_y << " isn't contained within " << width << "x" << height << " base image." << std::endl;
			return 1;
		}

		for (uint32_t y = 0; y < over_height; ++y) {
			for (uint32_t x = 0; x < over_width; ++x) {
				data[(y + over_y) * width + (x + over_x)] = over[y * over_width + x];
			}
		}
	}

	save_png(argv[argc-1], width, height, &data[0], UpperLeftOrigin);

	return 0;
}
