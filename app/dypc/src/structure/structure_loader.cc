#include "structure_loader.h"
#include <algorithm>

namespace dypc {

void structure_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	unsigned counter = 0;
	const unsigned repeat_max = 10;
	do {
		count = this->extract_points_(points, capacity, req);
		adapt_result_ = this->adapt_settings_(count, capacity);
		
		if(
			(adapt_result_ == adapt_continue_decreasing && count >= old_count_) ||
			(adapt_result_ == adapt_continue_increasing && count <= old_count_)
		) adapt_result_ = adapt_stabilize;

		old_count_ = count;
	} while(adaptive_ && (adapt_result_ == adapt_repeat) && (++counter < repeat_max));
		

	
	stat_memory_size_ = this->memory_size_();
	if(! initialized_stats_) {
		stat_file_size_ = this->file_size_();
		stat_total_points_ = this->total_points_();
		initialized_stats_ = true;
	}
}


}
