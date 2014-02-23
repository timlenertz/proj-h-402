#include "structure_loader.h"
#include <algorithm>

namespace dypc {

void structure_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	count = this->extract_points_(points, capacity, req);
}


}
