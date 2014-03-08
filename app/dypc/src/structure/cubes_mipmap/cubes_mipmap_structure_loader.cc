#include "cubes_mipmap_structure_loader.h"

namespace dypc {


double cubes_mipmap_structure_loader::get_setting(const std::string& setting) const {
	if(setting == "downsampling_start_distance") return downsampling_start_distance_;
	else if(setting == "downsampling_step_distance") return downsampling_step_distance_;
	else if(setting == "frustum_culling") return (frustum_culling_ ? 1.0 : 0.0);
	else throw std::invalid_argument("Invalid loader setting");
}

void cubes_mipmap_structure_loader::set_setting(const std::string& setting, double value) {
	if(setting == "downsampling_start_distance") downsampling_start_distance_ = value;
	else if(setting == "downsampling_step_distance") downsampling_step_distance_ = value;
	else if(setting == "frustum_culling") frustum_culling_ = (value != 0.0);
	else throw std::invalid_argument("Invalid loader setting");
}

}
