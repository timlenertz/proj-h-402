#include <cstdlib>
#include <cmath>
#include "random_model.h"

namespace dypc {

void random_model::rewind() {
	remaining_ = number_of_points_;
}

bool random_model::next_point(point& pt) {
	if(! remaining_) return false;
		
	float x = (side_ / RAND_MAX) * std::rand() - side_/2;
	float y = (side_ / RAND_MAX) * std::rand() - side_/2;
	float z = (side_ / RAND_MAX) * std::rand() - side_/2;

	pt = point(x, y, z);
	--remaining_;
	return true;
}

}
