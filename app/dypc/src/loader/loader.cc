#include "loader.h"

namespace dypc {

loader::~loader() { }

std::string loader::loader_name() const {
	return "Unnamed loader";
}

double loader::get_setting(const std::string& setting) const {
	if(setting == "adaptive") return adaptive_ ? 1.0 : 0.0;
	else throw std::invalid_argument("Invalid loader setting");
}

void loader::set_setting(const std::string& setting, double value) {
	if(setting == "adaptive") adaptive_ = (value != 0.0);
	else throw std::invalid_argument("Invalid loader setting");
}


}
