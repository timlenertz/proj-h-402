#include "loader.h"

namespace dypc {

bool loader::should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) {
	return
		glm::distance(request.position, previous.position) > minimal_update_distance_ ||
		request.orientation != previous.orientation;
}

std::string loader::loader_name() const {
	return "Unnamed loader";
}

double loader::get_setting(const std::string& setting) const {
	throw std::invalid_argument("Invalid loader setting");
}

void loader::set_setting(const std::string& setting, double value) {
	throw std::invalid_argument("Invalid loader setting");
}


}
