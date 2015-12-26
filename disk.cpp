#include "load_save_png.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <sstream>
#include <cmath>

int main(int argc, char **argv) {
	bool usage = false;

	float radius;

	if (argc != 3) {
		usage = true;
	} else {
		std::istringstream geom_str(argv[1]);
		char x;
		if (!(geom_str >> radius) || (geom_str >> x)) {
			usage = true;
		}
	}

	if (usage) {
		std::cerr << "Usage:\n"
			"\tdisk <radius> <out.png>\n"
			" Render a disk psf of the given radius, will always produce odd-sized image.\n"
		<< std::endl;
		return 1;
	}

	if (radius <= 0.0f) {
		std::cerr << "ERROR: Radius should be positive.\n" << std::endl;
		return 1;
	}

	uint32_t size = 0;
	while ((size + 0.5f) < radius) ++size;
	size = 2 * size + 1;

	std::vector< uint32_t > out(size * size);
	for (uint32_t y = 0; y < size; ++y) {
		for (uint32_t x = 0; x < size; ++x) {
			float at_x = (x + 0.5f) - (size * 0.5f);
			float at_y = (y + 0.5f) - (size * 0.5f);
			float dis = hypotf(at_x, at_y);
			if (dis <= radius) {
				out[y * size + x] = 0xffffffff;
			} else {
				out[y * size + x] = 0x00000000;
			}
		}
	}

	save_png(argv[2], size, size, &out[0], UpperLeftOrigin);

	return 0;
}
