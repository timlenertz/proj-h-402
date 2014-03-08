#include "cubes_structure_loader.h"
#include <iostream>

namespace dypc {

cubes_structure_loader::adapt_result_t cubes_structure_loader::adapt_settings_(std::size_t last_extracted, std::size_t capacity) {
	const std::size_t minimum_good = 0.8 * capacity;
	const std::size_t maximum_good = 0.9 * capacity;
	
	if(last_extracted == 0) return adapt_stabilize;
	
	adapt_result_t result;
		
	if(last_extracted > minimum_good && last_extracted < maximum_good) {
		return adapt_stabilize;
	} else if(last_extracted < minimum_good) {
		downsampling_distance_ += 5.0 * (1.0 - (float)(last_extracted / minimum_good));
		if(downsampling_distance_ > 500.0) downsampling_distance_ = 500.0;
		result = adapt_continue_increasing;
	} else {
		downsampling_distance_ -= 20.0 * (1.0 - (float)(maximum_good / last_extracted));
		if(downsampling_distance_ < 1.0) downsampling_distance_ = 1.0;
		result = adapt_continue_decreasing;
	}
	
	std::cout << downsampling_distance_ << std::endl;
	
	if(last_extracted >= capacity) return adapt_repeat;
	else return result;
}

double cubes_structure_loader::get_setting(const std::string& setting) const {
	if(setting == "downsampling_distance") return downsampling_distance_;
	else if(setting == "frustum_culling") return (frustum_culling_ ? 1.0 : 0.0);
	else if(setting == "downsampling_level") return downsampling_level_;
	else if(setting == "secondary_pass_distance") return secondary_pass_distance_;
	else throw std::invalid_argument("Invalid loader setting");
}

void cubes_structure_loader::set_setting(const std::string& setting, double value) {
	if(setting == "downsampling_distance") downsampling_distance_ = value;
	else if(setting == "frustum_culling") frustum_culling_ = (value != 0.0);
	else if(setting == "downsampling_level") downsampling_level_ = value;
	else if(setting == "secondary_pass_distance") secondary_pass_distance_ = value;
	else throw std::invalid_argument("Invalid loader setting");
}

}
