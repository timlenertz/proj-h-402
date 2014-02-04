#include "structure_loader.h"
#include <algorithm>

namespace dypc {

void structure_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {		
	count = this->extract_points_(points, capacity, req);
	
	stat_memory_size_ = this->memory_size_();
	if(! initialized_stats_) {
		stat_file_size_ = this->file_size_();
		stat_total_points_ = this->total_points_();
		initialized_stats_ = true;
	}
}


}
