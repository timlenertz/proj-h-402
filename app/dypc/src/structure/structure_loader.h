#ifndef DYPC_STRUCTURE_LOADER_H_
#define DYPC_STRUCTURE_LOADER_H_

#include "../point.h"
#include "../loader/loader.h"
#include "../util.h"
#include <stdexcept>
#include <string>
#include <map>
#include <cstring>
#include <cstdint>

namespace dypc {

class structure_loader : public loader {
public:
	static constexpr float initial_downsampling_setting = 20.0;
	
protected:
	float downsampling_setting_ = initial_downsampling_setting;
	static constexpr float minimal_update_distance_ = 1;

	void compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override {
		return
			glm::distance(request.position, previous.position) > minimal_update_distance_ ||
			request.orientation != previous.orientation;
	}
		
	virtual std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) = 0;
	void adapt_setting_(std::size_t last_extracted, std::size_t capacity);

public:
	float get_downsampling_setting() const { return downsampling_setting_; }
	void set_downsampling_setting(float d) { downsampling_setting_ = d; }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
};

}

#endif
