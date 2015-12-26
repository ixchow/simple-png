#include "load_save_png.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <sstream>

int main(int argc, char **argv) {
	bool usage = false;

	uint32_t crop_x,crop_y,crop_width,crop_height;

	if (argc != 4) {
		usage = true;
	} else {
		std::istringstream geom_str(argv[2]);
		char x,plus1,plus2;
		if (!(geom_str >> crop_width >> x >> crop_height >> plus1 >> crop_x >> plus2 >> crop_y) || x != 'x' || plus1 != '+' || plus2 != '+' || (geom_str >> x)) {
			usage = true;
		}
	}

	if (usage) {
		std::cerr << "Usage:\n"
			"\tcrop <in.png> <width>x<height>+<x>+<y> <out.png>\n"
			" Copy width x height section of in.png at x,y to out.png.\n"
			" (Note: origin is in upper left.)\n"
		<< std::endl;
		return 1;
	}

	std::vector< uint32_t > data;
	uint32_t width, height;
	if (!load_png(argv[1], width, height, data, UpperLeftOrigin)) {
		std::cerr << "Failed to load '" << argv[1] << "'" << std::endl;
		return 1;
	}
	if (uint32_t(crop_x + crop_width) > width || uint32_t(crop_y + crop_height) > height) {
		std::cerr << "Crop rectangle of size " << crop_width << "x" << crop_height << " at " << crop_x << ", " << crop_y << " isn't contained within " << width << "x" << height << " image." << std::endl;
		return 1;
	}

	std::vector< uint32_t > out_data(crop_width * crop_height);
	for (uint32_t y = 0; y < crop_height; ++y) {
		for (uint32_t x = 0; x < crop_width; ++x) {
			out_data[y * crop_width + x] = data[(y + crop_y) * width + (x + crop_x)];
		}
	}

	save_png(argv[3], crop_width, crop_height, &out_data[0], UpperLeftOrigin);

	return 0;
}
