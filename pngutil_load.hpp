#pragma once

#include <string>
#include <vector>

//Note: always uses upper-left origin.
bool pngutil_load(std::string name, uint32_t &width, uint32_t &height, std::vector< uint32_t > &data);
