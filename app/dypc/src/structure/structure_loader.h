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
protected:
	enum adapt_result_t { adapt_stabilize = 0, adapt_continue_decreasing, adapt_continue_increasing, adapt_repeat };

private:
	std::size_t old_count_ = 0;
	adapt_result_t adapt_result_ = adapt_stabilize;	
	
protected:
	static constexpr float minimal_update_distance_ = 1;

	void compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override {
		return
			(adapt_result_ != adapt_stabilize) ||
			glm::distance(request.position, previous.position) > minimal_update_distance_ ||
			request.orientation != previous.orientation;
	}
	
	virtual std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) = 0;
	virtual adapt_result_t adapt_settings_(std::size_t last_extracted, std::size_t capacity) { return adapt_stabilize; }
	
	virtual std::size_t memory_size_() const = 0;
	virtual std::size_t file_size_() const = 0;
	virtual std::size_t total_points_() const = 0;
};

}

#endif
