#ifndef DYPC_STRUCTURE_LOADER_H_
#define DYPC_STRUCTURE_LOADER_H_

#include "../point.h"
#include "../loader/loader.h"
#include "../statistics.h"
#include <stdexcept>
#include <string>
#include <map>

namespace dypc {

class structure_loader : public loader {
private:
	bool initialized_stats_ = false;
	statistics::item stat_file_size_;
	statistics::item stat_memory_size_;
	statistics::item stat_total_points_;

protected:
	static constexpr float minimal_update_distance_ = 1;

	void compute_points_(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) override {
		count = this->extract_points_(points, capacity, request);
		
		stat_memory_size_ = this->memory_size_();
		if(! initialized_stats_) {
			stat_file_size_ = this->file_size_();
			stat_total_points_ = this->total_points_();
			initialized_stats_ = true;
		}
	}
	
	bool should_compute_points_(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override {
		return
			glm::distance(request.position, previous.position) > minimal_update_distance_
			|| request.orientation != previous.orientation;
	}
	
	structure_loader() :
		stat_file_size_(statistics::add("Structure ROM", 0, statistics::file_size, statistics::rom_size)),
		stat_memory_size_(statistics::add("Structure Memory", 0, statistics::file_size, statistics::memory_size)),
		stat_total_points_(statistics::add("Total Points", 0, statistics::number, statistics::model_total_points)) { }

	virtual std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) = 0;
	
	virtual std::size_t memory_size_() const = 0;
	virtual std::size_t file_size_() const = 0;
	virtual std::size_t total_points_() const = 0;		
};

}

#endif
