#include "cubes_structure_loader.h"
#include <iostream>

namespace dypc {

double cubes_structure_loader::get_setting(const std::string& setting) const {
	if(setting == "frustum_culling") return (frustum_culling_ ? 1.0 : 0.0);
	else if(setting == "downsampling_levels") return downsampling_levels_;
	else if(setting == "downsampling_amount") return downsampling_amount_;
	else if(setting == "secondary_pass_distance") return secondary_pass_distance_;
	else return downsampling_loader::get_setting(setting);
}

void cubes_structure_loader::set_setting(const std::string& setting, double value) {	
	if(setting == "frustum_culling") frustum_culling_ = (value != 0.0);
	else if(setting == "downsampling_levels") downsampling_levels_ = value;
	else if(setting == "downsampling_amount") downsampling_amount_ = value;
	else if(setting == "secondary_pass_distance") secondary_pass_distance_ = value;
	else downsampling_loader::set_setting(setting, value);
}

}
