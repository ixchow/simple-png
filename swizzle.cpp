#include "load_save_png.hpp"
#include "pngutil_load.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>

int main(int argc, char **argv) {
	bool usage = false;

	uint8_t src[4];

	if (argc != 5) {
		usage = true;
	} else {
		std::string src_str(argv[3]);
		if (src_str.size() != 4) {
			usage = true;
		} else {
			std::map< char, uint8_t > dest;
			dest.insert(std::make_pair('r', 0));
			dest.insert(std::make_pair('g', 1));
			dest.insert(std::make_pair('b', 2));
			dest.insert(std::make_pair('a', 3));
			dest.insert(std::make_pair('R', 4));
			dest.insert(std::make_pair('G', 5));
			dest.insert(std::make_pair('B', 6));
			dest.insert(std::make_pair('A', 7));
			dest.insert(std::make_pair('x', 8));
			dest.insert(std::make_pair('y', 9));
			dest.insert(std::make_pair('z', 10));
			dest.insert(std::make_pair('w', 11));
			dest.insert(std::make_pair('X', 12));
			dest.insert(std::make_pair('Y', 13));
			dest.insert(std::make_pair('Z', 14));
			dest.insert(std::make_pair('W', 15));
			dest.insert(std::make_pair('0', 16));
			dest.insert(std::make_pair('1', 17));
			dest.insert(std::make_pair('*', 18));
			for (uint32_t i = 0; i < 4; ++i) {
				auto f = dest.find(src_str[i]);
				if (f == dest.end()) {
					usage = true;
					src[i] = 0;
				} else {
					src[i] = f->second;
				}
			}
	}
	}

	if (usage) {
		std::cerr << "Usage:\n"
			"\tswizzle <in1.png> <in2.png> <mask> <out.png>\n"
			" Where 'mask' uses four letters from rgba[RGBA] (or xyzw[XYZW] for inverse) to indicate the channels of the first [second] image to be placed in the output image."
		<< std::endl;
		return 1;
	}

	std::vector< uint32_t > data_a,data_b;
	uint32_t width_a, height_a, width_b, height_b;
	if (!pngutil_load(argv[1], width_a, height_a, data_a)) {
		std::cerr << "Failed to load '" << argv[1] << "'" << std::endl;
		return 1;
	}
	if (!pngutil_load(argv[2], width_b, height_b, data_b)) {
		std::cerr << "Failed to load '" << argv[2] << "'" << std::endl;
		return 1;
	}
	if (width_a != width_b || height_a != height_b) {
		std::cerr << "Loaded images are not the same size." << std::endl;
		return 1;
	}

	std::vector< uint32_t > data(data_a.size());

	const uint8_t *a = reinterpret_cast< const uint8_t * >(&data_a[0]);
	const uint8_t *b = reinterpret_cast< const uint8_t * >(&data_b[0]);
	uint8_t *out = reinterpret_cast< uint8_t * >(&data[0]);
	for (uint32_t i = 0; i < data.size(); ++i) {
		for (uint32_t c = 0; c < 4; ++c) {
			if (src[c] < 4) {
				out[i*4+c] = a[i*4+src[c]];
			} else if (src[c] < 8) {
				out[i*4+c] = b[i*4+src[c]-4];
			} else if (src[c] < 12) {
				out[i*4+c] = 255 - a[i*4+src[c]-8];
			} else if (src[c] < 16) {
				out[i*4+c] = 255 - b[i*4+src[c]-12];
			} else if (src[c] == 16) {
				out[i*4+c] = 0x00;
			} else if (src[c] == 17) {
				out[i*4+c] = 0xff;
			} else {
				assert(src[c] == 18);
				out[i*4+c] = (uint32_t(a[i*4+c]) * uint32_t(b[i*4+c])) / 255;
			}
		}
	}

	save_png(argv[4], width_a, height_a, &data[0], UpperLeftOrigin);

	return 0;
}
