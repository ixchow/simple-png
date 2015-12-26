#include "load_save_png.hpp"

#include <iostream>
#include <string>
#include <map>
#include <cassert>

int main(int argc, char **argv) {
	bool usage = false;

	if (argc != 4) {
		usage = true;
	}

	if (usage) {
		std::cerr << "Usage:\n"
			"\tblur <in.png> <psf.png> <out.png>\n"
			" Store in.png * psf.png to out.png .\n"
			" NOTE: in.png will be nearest-neighbor padded,\n"
			"       psf.png must be of odd size and will be normalized.\n"
		<< std::endl;
		return 1;
	}

	std::vector< uint32_t > in,psf;
	uint32_t in_width, in_height, psf_width, psf_height;
	if (!load_png(argv[1], in_width, in_height, in, UpperLeftOrigin)) {
		std::cerr << "Failed to load '" << argv[1] << "'" << std::endl;
		return 1;
	}
	if (!load_png(argv[2], psf_width, psf_height, psf, UpperLeftOrigin)) {
		std::cerr << "Failed to load '" << argv[2] << "'" << std::endl;
		return 1;
	}
	if (!(psf_width & 1) || !(psf_height & 1)) {
		std::cerr << "ERROR: PSF is not of odd size." << std::endl;
		return 1;
	}
	//I guess not technically an error but:
	if (in_width == 0 || in_height == 0) {
		save_png(argv[3], in_width, in_height, &in[0], UpperLeftOrigin);
		return 0;
	}

	uint32_t padded_width = in_width + psf_width - 1;
	uint32_t padded_height = in_height + psf_height - 1;
	std::vector< uint32_t > padded(padded_width * padded_height);
	for (uint32_t y = 0; y < padded_height; ++y) {
		for (uint32_t x = 0; x < padded_width; ++x) {
			int32_t src_y = int32_t(y) - int32_t(psf_height / 2);
			if (src_y < 0) src_y = 0;
			else if (uint32_t(src_y) >= in_height) src_y = in_height - 1;

			int32_t src_x = int32_t(x) - int32_t(psf_width / 2);
			if (src_x < 0) src_x = 0;
			else if (uint32_t(src_x) >= in_width) src_x = in_width - 1;

			padded[y * padded_width + x] = in[src_y * in_width + src_x];
		}
	}

	std::vector< uint32_t > out(in.size(), 0);

	for (uint32_t shift = 0; shift < 32; shift += 8) {
		uint32_t psf_sum = 0;
		for (auto p = psf.begin(); p != psf.end(); ++p) {
			psf_sum += (*p >> shift) & 0xff;
		}
		if (psf_sum == 0) {
			std::cerr << "ERROR: PSF channel sums to zero." << std::endl;
			return 1;
		}
		if (psf_sum > 0x00ffffff) {
			std::cerr << "ERROR: PSF sum doesn't have enough headroom." << std::endl;
			return 1;
		}
		for (uint32_t y = 0; y < in_height; ++y) {
			for (uint32_t x = 0; x < in_width; ++x) {
				uint32_t px_sum = 0;
				for (uint32_t oy = 0; oy < psf_width; ++oy) {
					for (uint32_t ox = 0; ox < psf_height; ++ox) {
						px_sum += ((padded[(y + oy) * padded_width + (x + ox)] >> shift) & 0xff) * ((psf[oy * psf_width + ox] >> shift) & 0xff);
					}
				}
				out[y * in_width + x] |= ((px_sum / psf_sum) << shift);
			}
		}
	}

	save_png(argv[3], in_width, in_height, &out[0], UpperLeftOrigin);

	return 0;
}
