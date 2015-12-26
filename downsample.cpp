#include "load_save_png.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <sstream>

int main(int argc, char **argv) {
	bool usage = false;

	uint32_t factor = 0;

	if (argc != 4) {
		usage = true;
	} else {
		std::istringstream geom_str(argv[2]);
		char x;
		if (!(geom_str >> factor) || (geom_str >> x)) {
			usage = true;
		}
	}

	if (usage) {
		std::cerr << "Usage:\n"
			"\tdownsample <in.png> <factor> <out.png>\n"
			" Average each factor^2 box of pixels in in.png to create out.png.\n"
		<< std::endl;
		return 1;
	}

	if (factor < 2) {
		std::cerr << "Downsample factor " << factor << " (less than two) doesn't make sense." << std::endl;
		return 1;
	}

	std::vector< uint32_t > in;
	uint32_t width, height;
	if (!load_png(argv[1], width, height, in, UpperLeftOrigin)) {
		std::cerr << "Failed to load '" << argv[1] << "'" << std::endl;
		return 1;
	}
	if (width % factor != 0 || height % factor != 0) {
		std::cerr << "Image size " << width << "x" << height << " not evenly divisible by " << factor << "." << std::endl;
		return 1;
	}
	if (width < factor || height < factor) {
		std::cerr << "Image is too small." << std::endl;
		return 1;
	}

	uint32_t out_width = (width - 1) / factor + 1;
	uint32_t out_height = (height - 1) / factor + 1;
	std::vector< uint32_t > out(out_width * out_height);
	for (uint32_t y = 0; y < out_height; ++y) {
		for (uint32_t x = 0; x < out_width; ++x) {
			uint32_t sum[4] = {0,0,0,0};
			for (uint32_t oy = 0; oy < factor; ++oy) {
				for (uint32_t ox = 0; ox < factor; ++ox) {
					uint32_t px = in[(y * factor + oy) * width + (x * factor + ox)];
					sum[0] += px & 0xff;
					sum[1] += (px >> 8) & 0xff;
					sum[2] += (px >> 16) & 0xff;
					sum[3] += (px >> 24);
				}
			}
			out[y * out_width + x] =
				  (sum[0] / (factor * factor))
				| (sum[1] / (factor * factor)) << 8
				| (sum[2] / (factor * factor)) << 16
				| (sum[3] / (factor * factor)) << 24;
		}
	}

	save_png(argv[3], out_width, out_height, &out[0], UpperLeftOrigin);

	return 0;
}
