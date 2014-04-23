#include "downsampling.h"

namespace dypc {

float choose_downsampling_level_continuous(std::size_t levels, float distance, float setting) {
	const float start_distance_exponent = 1.3;
	float start_distance = std::pow(setting, start_distance_exponent);
	float step_distance = setting;
	
	float i = 0;
	if(distance > start_distance) i = (distance - start_distance) / step_distance;
	if(i > levels - 1) i = levels - 1;
	return i;
}

float downsampling_ratio_for_level(float i, std::size_t levels, std::size_t total_points, std::size_t minimum, float amount) {
	assert(i >= 0.0 && i <= levels - 1);
	if(total_points == 0) return 1.0;
	float minimum_ratio = (float)minimum / total_points;
	if(minimum_ratio > 1.0) minimum_ratio = 1.0;
	float x = i / (levels - 1);
	return 1.0 - (1.0 - minimum_ratio)*std::pow(x, amount);
}

downsampling_ratios_t determine_downsampling_ratios(std::size_t levels, std::size_t total_points, std::size_t minimum, float amount) {
	if(total_points == 0) return downsampling_ratios_t(levels, 1.0);
	
	downsampling_ratios_t result(levels);
	
	result[levels - 1] = (float)minimum / total_points;;
	for(std::ptrdiff_t i = levels - 1; i >= 1; --i) result[i] = downsampling_ratio_for_level(i, levels, total_points, minimum, amount);
	result[0] = 1.0;
	
	for(auto r : result) std::cout << r << ", ";
	std::cout << std::endl;
	
	return result;
}

}
