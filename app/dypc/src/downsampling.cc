#include "downsampling.h"

namespace dypc {

float choose_downsampling_level_continuous(std::size_t levels, float distance, float setting) {
	const float start_distance_exponent = 1.3;
	float start_distance = std::pow(setting, start_distance_exponent);
	float step_distance = setting;
	
	float i = 0;
	if(distance > start_distance) i = (distance - start_distance) / step_distance;
	if(i >= levels) i = levels - 1;
	return i;
}

}
