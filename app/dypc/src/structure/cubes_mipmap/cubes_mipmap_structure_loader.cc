#include "cubes_mipmap_structure_loader.h"

namespace dypc {


double cubes_mipmap_structure_loader::get_setting(const std::string& setting) const {
	if(setting == "frustum_culling") return (frustum_culling_ ? 1.0 : 0.0);
	else return structure_loader::get_setting(setting);
}

void cubes_mipmap_structure_loader::set_setting(const std::string& setting, double value) {
	if(setting == "frustum_culling") frustum_culling_ = (value != 0.0);
	else structure_loader::set_setting(setting, value);
}

}
