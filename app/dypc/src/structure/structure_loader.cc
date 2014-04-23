#include "structure_loader.h"
#include <algorithm>

namespace dypc {

void structure_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	do {
		count = this->extract_points_(points, capacity, req);
		adapt_setting_(count, capacity);
	} while(count > capacity);
}

void structure_loader::adapt_setting_(std::size_t last_extracted, std::size_t capacity) {
	
}

double structure_loader::get_setting(const std::string& setting) const {
	if(setting == "downsampling_setting") return downsampling_setting_;
	else return loader::get_setting(setting);
}

void structure_loader::set_setting(const std::string& setting, double value) {
	if(setting == "downsampling_setting") downsampling_setting_ = value;
	else loader::set_setting(setting, value);
}

}
